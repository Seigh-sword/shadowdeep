#pragma once
#include <string>
#include <vector>

namespace shadowdeep {

enum class RegionId : int {
    ForgottenCellars = 0,
    GoblinWarrens = 1,
    SunkenCrypts = 2,
    FungalDepths = 3,
    CrystalCaverns = 4,
    DrownedHalls = 5,
    InfernalFoundry = 6,
    AshenFortress = 7,
    AbyssalTemple = 8,
    Shadowdeep = 9,
    Count = 10
};

struct RegionInfo {
    RegionId id;
    std::string stableId;
    std::string name;
    std::string description;
    int minDepth;
    int maxDepth;
    std::string theme;
};

const std::vector<RegionInfo>& allRegions();
const RegionInfo& regionInfo(RegionId id);
RegionId regionForDepth(int depth);
std::string regionNameForDepth(int depth);

}
