#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace shadowdeep {

struct BestiaryEntry {
    std::string stableId;
    std::string name;
    bool seen = false;
    bool killed = false;
    int killCount = 0;
    int maxDamageDealt = 0;
    int maxDamageTaken = 0;
    bool knowsResistances = false;
    bool knowsAbilities = false;
};

class Bestiary {
public:
    void see(const std::string& stableId);
    void kill(const std::string& stableId, int dmgDealt, int dmgTaken);
    const BestiaryEntry* get(const std::string& stableId) const;
    std::vector<BestiaryEntry> all() const;
    int totalSeen() const;
    int totalKilled() const;

private:
    std::unordered_map<std::string, BestiaryEntry> entries_;
};

}
