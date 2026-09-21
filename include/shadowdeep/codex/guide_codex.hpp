#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace shadowdeep {

enum class GuideCategory : uint8_t {
    Weapons = 0,
    Armor = 1,
    Potions = 2,
    Scrolls = 3,
    Monsters = 4,
    Biomes = 5,
    Lore = 6,
    Secrets = 7,
    Count = 8
};

struct GuideEntry {
    std::string id;
    std::string title;
    std::string description;
    GuideCategory category = GuideCategory::Weapons;
    bool unlocked = false;
    int fragmentCount = 0;
    int fragmentsRequired = 1;
    std::string sourceItem;
    std::string loreText;
};

struct LoreFragment {
    std::string id;
    std::string title;
    std::string text;
    int depthFound = 0;
    bool isBossDrop = false;
    std::string bossSource;
};

class GuideCodex {
public:
    GuideCodex();

    void addFragment(const std::string& fragmentId, const std::string& sourceItem = "");
    void unlockGuide(const std::string& guideId);
    void addLore(const LoreFragment& lore);
    void addLore(const std::string& id, const std::string& title, const std::string& text, int depth, bool bossDrop = false, const std::string& boss = "");

    bool isUnlocked(const std::string& guideId) const;
    const GuideEntry* getGuide(const std::string& id) const;
    std::vector<GuideEntry> allGuides() const;
    std::vector<GuideEntry> unlockedGuides() const;
    std::vector<GuideEntry> guidesByCategory(GuideCategory cat) const;

    const LoreFragment* getLore(const std::string& id) const;
    std::vector<LoreFragment> allLore() const;
    std::vector<LoreFragment> bossLore() const;

    int totalFragments() const;
    int totalGuides() const;
    int unlockedCount() const;
    int loreCount() const;

    void initializeDefaults();

    std::string categoryName(GuideCategory cat) const;

private:
    std::unordered_map<std::string, GuideEntry> guides_;
    std::unordered_map<std::string, LoreFragment> lores_;
    std::unordered_map<std::string, int> fragmentCounts_;
    int totalFragmentsCollected_ = 0;
};

}
