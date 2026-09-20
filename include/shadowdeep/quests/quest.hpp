#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace shadowdeep {

enum class QuestType : uint8_t {
    Rescue = 0,
    Hunt = 1,
    BossHunt = 2,
    ItemRecovery = 3,
    Exploration = 4,
    Collection = 5,
    Count = 6
};

enum class QuestStatus : uint8_t {
    Available = 0,
    Active = 1,
    Completed = 2,
    Failed = 3
};

struct QuestObjective {
    std::string description;
    int targetCount = 1;
    int currentCount = 0;
    bool completed = false;
    std::string targetId;
};

struct Quest {
    std::string stableId;
    std::string name;
    std::string description;
    QuestType type = QuestType::Hunt;
    QuestStatus status = QuestStatus::Available;
    std::vector<QuestObjective> objectives;
    int goldReward = 0;
    int rubyReward = 0;
    int xpReward = 0;
    std::string giverId;
    int minDepth = 1;
    int maxDepth = 30;
    bool mandatory = false;

    bool isComplete() const;
    void updateProgress(const std::string& targetId, int amount = 1);
};

class QuestManager {
public:
    void addQuest(const Quest& q);
    void activate(const std::string& id);
    void complete(const std::string& id);
    void fail(const std::string& id);
    void update(const std::string& targetId, int amount = 1);

    const std::vector<Quest>& all() const;
    std::vector<Quest> active() const;
    std::vector<Quest> available() const;

private:
    std::vector<Quest> quests_;
};

}
