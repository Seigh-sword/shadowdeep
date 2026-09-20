#include "shadowdeep/quests/quest.hpp"
#include <algorithm>

namespace shadowdeep {

bool Quest::isComplete() const {
    for (auto& o : objectives) if (!o.completed) return false;
    return !objectives.empty();
}

void Quest::updateProgress(const std::string& targetId, int amount) {
    for (auto& o : objectives) {
        if (o.targetId == targetId) {
            o.currentCount += amount;
            if (o.currentCount >= o.targetCount) {
                o.currentCount = o.targetCount;
                o.completed = true;
            }
        }
    }
}

void QuestManager::addQuest(const Quest& q) {
    quests_.push_back(q);
}

void QuestManager::activate(const std::string& id) {
    for (auto& q : quests_) if (q.stableId == id) q.status = QuestStatus::Active;
}

void QuestManager::complete(const std::string& id) {
    for (auto& q : quests_) if (q.stableId == id) q.status = QuestStatus::Completed;
}

void QuestManager::fail(const std::string& id) {
    for (auto& q : quests_) if (q.stableId == id) q.status = QuestStatus::Failed;
}

void QuestManager::update(const std::string& targetId, int amount) {
    for (auto& q : quests_) if (q.status == QuestStatus::Active) q.updateProgress(targetId, amount);
}

const std::vector<Quest>& QuestManager::all() const { return quests_; }

std::vector<Quest> QuestManager::active() const {
    std::vector<Quest> res;
    for (auto& q : quests_) if (q.status == QuestStatus::Active) res.push_back(q);
    return res;
}

std::vector<Quest> QuestManager::available() const {
    std::vector<Quest> res;
    for (auto& q : quests_) if (q.status == QuestStatus::Available) res.push_back(q);
    return res;
}

}
