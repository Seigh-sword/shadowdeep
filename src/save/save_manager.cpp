#include "shadowdeep/save/save_manager.hpp"
#include "shadowdeep/save/binary_reader.hpp"
#include "shadowdeep/save/binary_writer.hpp"
#include "shadowdeep/save/save_header.hpp"
#include "shadowdeep/version.hpp"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <ctime>

namespace shadowdeep {

namespace fs = std::filesystem;

SaveManager::SaveManager(const AppPaths& paths) : paths_(paths) {}

std::vector<EntryMetadata> SaveManager::listEntries() {
    std::vector<EntryMetadata> res;
    try {
        if (!fs::exists(paths_.savesDir)) return res;
        for (auto& entry : fs::directory_iterator(paths_.savesDir)) {
            if (!entry.is_regular_file()) continue;
            auto path = entry.path();
            if (path.extension() != ".sav") continue;
            std::string id = path.stem().string();
            auto meta = getMetadata(id);
            if (meta) res.push_back(*meta);
            else {
                EntryMetadata m;
                m.entryId = id;
                m.entryName = id;
                m.status = EntryStatus::Corrupted;
                m.valid = false;
                m.fileSize = fs::file_size(path);
                res.push_back(m);
            }
        }
    } catch (...) {}
    std::sort(res.begin(), res.end(), [](const EntryMetadata& a, const EntryMetadata& b){
        return a.lastPlayedTimestamp > b.lastPlayedTimestamp;
    });
    return res;
}

std::optional<EntryMetadata> SaveManager::getMetadata(const std::string& entryId) {
    try {
        auto path = entryPath(entryId);
        if (!fs::exists(path)) return std::nullopt;
        std::ifstream f(path, std::ios::binary);
        if (!f) return std::nullopt;
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (data.size() < 64) return std::nullopt;

        BinaryReader reader(data);
        uint32_t magic = 0;
        if (!reader.readU32(magic)) return std::nullopt;
        if (magic != kSaveMagic) return std::nullopt;

        uint16_t containerVer = 0, schemaVer = 0;
        if (!reader.readU16(containerVer)) return std::nullopt;
        if (!reader.readU16(schemaVer)) return std::nullopt;

        std::array<uint8_t, 16> uuid{};
        for (int i = 0; i < 16; ++i) {
            uint8_t b = 0;
            if (!reader.readU8(b)) return std::nullopt;
            uuid[i] = b;
        }

        uint64_t creation = 0, lastSave = 0, payloadSize = 0;
        if (!reader.readU64(creation)) return std::nullopt;
        if (!reader.readU64(lastSave)) return std::nullopt;
        if (!reader.readU64(payloadSize)) return std::nullopt;

        uint8_t comp = 0;
        if (!reader.readU8(comp)) return std::nullopt;
        uint8_t res[3]{};
        for (int i = 0; i < 3; ++i) if (!reader.readU8(res[i])) return std::nullopt;

        uint32_t headerCrc = 0, payloadCrc = 0;
        if (!reader.readU32(headerCrc)) return std::nullopt;
        if (!reader.readU32(payloadCrc)) return std::nullopt;

        EntryMetadata meta;
        meta.entryId = entryId;
        meta.saveSchema = schemaVer;
        meta.creationTimestamp = static_cast<int64_t>(creation);
        meta.lastPlayedTimestamp = static_cast<int64_t>(lastSave);
        meta.fileSize = data.size();
        meta.gameVersion = std::string(kGameVersion);
        meta.valid = true;

        if (schemaVer > kSaveSchemaVersion) {
            meta.status = EntryStatus::Incompatible;
            return meta;
        }

        if (reader.remaining() < 16) {
            meta.entryName = entryId;
            return meta;
        }

        std::string entryName, charName, className, regionName, difficulty, seed;
        int32_t level = 1, depth = 1;
        int64_t playTime = 0;
        int32_t gold = 0, rubies = 0, kills = 0;
        bool hasAmulet = false;
        uint8_t status = 0;

        if (!reader.readString(entryName)) entryName = entryId;
        if (!reader.readString(charName)) charName = "Unknown";
        if (!reader.readString(className)) className = "Unknown";
        reader.readI32(level);
        reader.readI32(depth);
        reader.readString(regionName);
        reader.readI64(playTime);
        reader.readString(difficulty);
        reader.readString(seed);
        reader.readU8(status);
        reader.readI32(gold);
        reader.readI32(rubies);
        reader.readI32(kills);
        reader.readBool(hasAmulet);

        meta.entryName = entryName;
        meta.characterName = charName;
        meta.className = className;
        meta.level = level;
        meta.depth = depth;
        meta.regionName = regionName;
        meta.playTimeMs = playTime;
        meta.difficulty = difficulty;
        meta.seed = seed;
        meta.status = static_cast<EntryStatus>(status);
        meta.gold = gold;
        meta.rubies = rubies;
        meta.kills = kills;
        meta.hasAmulet = hasAmulet;

        return meta;
    } catch (...) {
        return std::nullopt;
    }
}

bool SaveManager::entryExists(const std::string& entryId) const {
    return fs::exists(entryPath(entryId));
}

fs::path SaveManager::entryPath(const std::string& entryId) const {
    return paths_.savesDir / (entryId + ".sav");
}

std::string SaveManager::generateUuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
    char buf[33];
    std::snprintf(buf, sizeof(buf), "%08x%08x%08x%08x", dis(gen), dis(gen), dis(gen), dis(gen));
    return std::string(buf);
}

bool SaveManager::createEntry(const std::string& entryName, const std::string& characterName, int pclass, uint32_t seed, const std::string& difficulty) {
    try {
        std::string id = generateUuid();
        auto path = entryPath(id);
        fs::create_directories(paths_.savesDir);

        BinaryWriter writer;
        writer.writeU32(kSaveMagic);
        writer.writeU16(kContainerFormatVersion);
        writer.writeU16(kSaveSchemaVersion);
        for (int i = 0; i < 16; ++i) writer.writeU8(static_cast<uint8_t>(rand() % 256));
        uint64_t now = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        writer.writeU64(now);
        writer.writeU64(now);
        writer.writeU64(0);
        writer.writeU8(0);
        writer.writeU8(0); writer.writeU8(0); writer.writeU8(0);
        writer.writeU32(0);
        writer.writeU32(0);

        writer.writeString(entryName);
        writer.writeString(characterName);
        writer.writeString(std::to_string(pclass));
        writer.writeI32(1);
        writer.writeI32(1);
        writer.writeString("Forgotten Cellars");
        writer.writeI64(0);
        writer.writeString(difficulty);
        writer.writeString(std::to_string(seed));
        writer.writeU8(static_cast<uint8_t>(EntryStatus::Active));
        writer.writeI32(0);
        writer.writeI32(0);
        writer.writeI32(0);
        writer.writeBool(false);

        std::vector<uint8_t> payload;
        payload.push_back(0);

        std::string tmpPath = path.string() + ".tmp";
        std::ofstream f(tmpPath, std::ios::binary);
        if (!f) return false;
        auto& d = writer.data();
        f.write(reinterpret_cast<const char*>(d.data()), d.size());
        f.write(reinterpret_cast<const char*>(payload.data()), payload.size());
        f.flush();
        f.close();

        fs::rename(fs::path(tmpPath), path);
        return true;
    } catch (...) {
        return false;
    }
}

bool SaveManager::deleteEntry(const std::string& entryId, bool moveToTrash) {
    try {
        auto path = entryPath(entryId);
        if (!fs::exists(path)) return false;
        if (moveToTrash) {
            auto trash = trashDir();
            fs::create_directories(trash);
            auto dest = trash / (entryId + ".sav");
            fs::rename(path, dest);
        } else {
            fs::remove(path);
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool SaveManager::renameEntry(const std::string& entryId, const std::string& newName) {
    (void)entryId;
    (void)newName;
    return false;
}

bool SaveManager::duplicateEntry(const std::string& entryId, const std::string& newName) {
    try {
        auto src = entryPath(entryId);
        if (!fs::exists(src)) return false;
        std::string newId = generateUuid();
        auto dst = entryPath(newId);
        fs::copy_file(src, dst);
        (void)newName;
        return true;
    } catch (...) {
        return false;
    }
}

bool SaveManager::backupEntry(const std::string& entryId) {
    try {
        auto src = entryPath(entryId);
        if (!fs::exists(src)) return false;
        fs::create_directories(paths_.backupsDir);
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        char buf[64];
#ifdef _WIN32
        std::tm tmBuf{};
        localtime_s(&tmBuf, &t);
        std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmBuf);
#else
        std::tm tmBuf{};
        localtime_r(&t, &tmBuf);
        std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tmBuf);
#endif
        auto dest = paths_.backupsDir / (entryId + "_" + buf + ".bak");
        fs::copy_file(src, dest);

        auto backups = listBackups(entryId);
        if (backups.size() > 10) {
            std::sort(backups.begin(), backups.end());
            for (size_t i = 0; i < backups.size() - 10; ++i) {
                try { fs::remove(backups[i]); } catch (...) {}
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<fs::path> SaveManager::listBackups(const std::string& entryId) {
    std::vector<fs::path> res;
    try {
        if (!fs::exists(paths_.backupsDir)) return res;
        for (auto& e : fs::directory_iterator(paths_.backupsDir)) {
            if (!e.is_regular_file()) continue;
            std::string name = e.path().filename().string();
            if (name.rfind(entryId, 0) == 0) res.push_back(e.path());
        }
    } catch (...) {}
    return res;
}

bool SaveManager::restoreBackup(const std::string& entryId, const fs::path& backupPath) {
    try {
        auto dest = entryPath(entryId);
        fs::copy_file(backupPath, dest, fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool SaveManager::exportEntry(const std::string& entryId, const fs::path& dest) {
    try {
        auto src = entryPath(entryId);
        if (!fs::exists(src)) return false;
        if (dest.has_parent_path()) fs::create_directories(dest.parent_path());
        fs::copy_file(src, dest, fs::copy_options::overwrite_existing);
        return true;
    } catch (...) {
        return false;
    }
}

bool SaveManager::importEntry(const fs::path& src, std::string& outNewId) {
    try {
        if (!fs::exists(src)) return false;
        if (src.string().find("..") != std::string::npos) return false;

        std::ifstream f(src, std::ios::binary);
        if (!f) return false;
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
        if (data.size() < 16) return false;
        if (data.size() > 100 * 1024 * 1024) return false;

        BinaryReader reader(data);
        uint32_t magic = 0;
        if (!reader.readU32(magic)) return false;
        if (magic != kSaveMagic) return false;

        std::string newId = generateUuid();
        auto dest = entryPath(newId);
        fs::create_directories(paths_.savesDir);
        fs::copy_file(src, dest);
        outNewId = newId;
        return true;
    } catch (...) {
        return false;
    }
}

fs::path SaveManager::trashDir() const {
    return paths_.dataRoot / "trash";
}

}
