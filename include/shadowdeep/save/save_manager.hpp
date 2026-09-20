#pragma once
#include <filesystem>
#include <vector>
#include <optional>
#include "../platform/paths.hpp"
#include "entry_metadata.hpp"

namespace shadowdeep {

class SaveManager {
public:
    explicit SaveManager(const AppPaths& paths);

    std::vector<EntryMetadata> listEntries();
    std::optional<EntryMetadata> getMetadata(const std::string& entryId);

    bool entryExists(const std::string& entryId) const;
    std::filesystem::path entryPath(const std::string& entryId) const;

    bool createEntry(const std::string& entryName, const std::string& characterName, int pclass, uint32_t seed, const std::string& difficulty);
    bool deleteEntry(const std::string& entryId, bool moveToTrash = true);
    bool renameEntry(const std::string& entryId, const std::string& newName);
    bool duplicateEntry(const std::string& entryId, const std::string& newName);

    bool backupEntry(const std::string& entryId);
    std::vector<std::filesystem::path> listBackups(const std::string& entryId);
    bool restoreBackup(const std::string& entryId, const std::filesystem::path& backupPath);

    bool exportEntry(const std::string& entryId, const std::filesystem::path& dest);
    bool importEntry(const std::filesystem::path& src, std::string& outNewId);

    std::filesystem::path trashDir() const;

private:
    AppPaths paths_;
    std::string generateUuid();
};

}
