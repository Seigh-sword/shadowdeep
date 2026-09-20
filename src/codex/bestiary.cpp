#include "shadowdeep/codex/bestiary.hpp"

namespace shadowdeep {

void Bestiary::see(const std::string& stableId) {
    auto& e = entries_[stableId];
    e.stableId = stableId;
    e.seen = true;
    if (e.name.empty()) e.name = stableId;
}

void Bestiary::kill(const std::string& stableId, int dmgDealt, int dmgTaken) {
    auto& e = entries_[stableId];
    e.stableId = stableId;
    e.seen = true;
    e.killed = true;
    e.killCount++;
    if (dmgDealt > e.maxDamageDealt) e.maxDamageDealt = dmgDealt;
    if (dmgTaken > e.maxDamageTaken) e.maxDamageTaken = dmgTaken;
    if (e.killCount >= 3) e.knowsAbilities = true;
    if (e.killCount >= 5) e.knowsResistances = true;
}

const BestiaryEntry* Bestiary::get(const std::string& stableId) const {
    auto it = entries_.find(stableId);
    if (it == entries_.end()) return nullptr;
    return &it->second;
}

std::vector<BestiaryEntry> Bestiary::all() const {
    std::vector<BestiaryEntry> res;
    for (auto& kv : entries_) res.push_back(kv.second);
    return res;
}

int Bestiary::totalSeen() const {
    int c = 0;
    for (auto& kv : entries_) if (kv.second.seen) c++;
    return c;
}

int Bestiary::totalKilled() const {
    int c = 0;
    for (auto& kv : entries_) if (kv.second.killed) c++;
    return c;
}

}
