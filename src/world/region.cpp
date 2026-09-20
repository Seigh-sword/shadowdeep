#include "shadowdeep/world/region.hpp"

namespace shadowdeep {

const std::vector<RegionInfo>& allRegions() {
    static std::vector<RegionInfo> regions = {
        {RegionId::ForgottenCellars, "region.forgotten_cellars", "Forgotten Cellars", "Damp stone and forgotten supplies.", 1, 3, "cellar"},
        {RegionId::GoblinWarrens, "region.goblin_warrens", "Goblin Warrens", "Crude tunnels stinking of goblin camps.", 3, 6, "warrens"},
        {RegionId::SunkenCrypts, "region.sunken_crypts", "Sunken Crypts", "Ancient burial halls flooded with shadows.", 5, 9, "crypt"},
        {RegionId::FungalDepths, "region.fungal_depths", "Fungal Depths", "Bioluminescent fungi and spore clouds.", 7, 12, "fungal"},
        {RegionId::CrystalCaverns, "region.crystal_caverns", "Crystal Caverns", "Glittering crystals humming with arcane power.", 10, 15, "crystal"},
        {RegionId::DrownedHalls, "region.drowned_halls", "Drowned Halls", "Waterlogged corridors and sunken statues.", 13, 18, "drowned"},
        {RegionId::InfernalFoundry, "region.infernal_foundry", "Infernal Foundry", "Molten metal and infernal forges.", 16, 22, "foundry"},
        {RegionId::AshenFortress, "region.ashen_fortress", "Ashen Fortress", "Charred ramparts of a fallen stronghold.", 20, 26, "ashen"},
        {RegionId::AbyssalTemple, "region.abyssal_temple", "Abyssal Temple", "A temple dedicated to void gods.", 24, 29, "abyssal"},
        {RegionId::Shadowdeep, "region.shadowdeep", "Shadowdeep", "The heart of darkness where the Amulet rests.", 28, 30, "shadowdeep"},
    };
    return regions;
}

const RegionInfo& regionInfo(RegionId id) {
    auto& all = allRegions();
    for (auto& r : all) {
        if (r.id == id) return r;
    }
    return all[0];
}

RegionId regionForDepth(int depth) {
    if (depth <= 3) return RegionId::ForgottenCellars;
    if (depth <= 6) return RegionId::GoblinWarrens;
    if (depth <= 9) return RegionId::SunkenCrypts;
    if (depth <= 12) return RegionId::FungalDepths;
    if (depth <= 15) return RegionId::CrystalCaverns;
    if (depth <= 18) return RegionId::DrownedHalls;
    if (depth <= 22) return RegionId::InfernalFoundry;
    if (depth <= 26) return RegionId::AshenFortress;
    if (depth <= 29) return RegionId::AbyssalTemple;
    return RegionId::Shadowdeep;
}

std::string regionNameForDepth(int depth) {
    return regionInfo(regionForDepth(depth)).name;
}

}
