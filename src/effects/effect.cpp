#include "shadowdeep/effects/effect.hpp"
#include <algorithm>

namespace shadowdeep {

std::string StatusEffect::stableId() const {
    switch (id) {
        case EffectId::Poison: return "effect.poison";
        case EffectId::Bleeding: return "effect.bleeding";
        case EffectId::Burning: return "effect.burning";
        case EffectId::Chilled: return "effect.chilled";
        case EffectId::Frozen: return "effect.frozen";
        case EffectId::Stunned: return "effect.stunned";
        case EffectId::Confused: return "effect.confused";
        case EffectId::Blinded: return "effect.blinded";
        case EffectId::Rooted: return "effect.rooted";
        case EffectId::Haste: return "effect.haste";
        case EffectId::Slow: return "effect.slow";
        case EffectId::Regeneration: return "effect.regeneration";
        case EffectId::Shielded: return "effect.shielded";
        case EffectId::Cursed: return "effect.cursed";
        case EffectId::Blessed: return "effect.blessed";
        case EffectId::Fear: return "effect.fear";
        case EffectId::Strength: return "effect.strength";
        default: return "effect.none";
    }
}

std::string StatusEffect::displayName() const {
    switch (id) {
        case EffectId::Poison: return "Poisoned";
        case EffectId::Bleeding: return "Bleeding";
        case EffectId::Burning: return "Burning";
        case EffectId::Chilled: return "Chilled";
        case EffectId::Frozen: return "Frozen";
        case EffectId::Stunned: return "Stunned";
        case EffectId::Confused: return "Confused";
        case EffectId::Blinded: return "Blinded";
        case EffectId::Rooted: return "Rooted";
        case EffectId::Haste: return "Hasted";
        case EffectId::Slow: return "Slowed";
        case EffectId::Regeneration: return "Regenerating";
        case EffectId::Shielded: return "Shielded";
        case EffectId::Cursed: return "Cursed";
        case EffectId::Blessed: return "Blessed";
        case EffectId::Fear: return "Feared";
        case EffectId::Strength: return "Strength";
        default: return "None";
    }
}

void EffectManager::add(EffectId id, int duration, int power, std::string source) {
    for (auto& e : effects_) {
        if (e.id == id) {
            e.duration = std::max(e.duration, duration);
            e.power = std::max(e.power, power);
            e.stacks++;
            return;
        }
    }
    StatusEffect ne;
    ne.id = id;
    ne.duration = duration;
    ne.power = power;
    ne.source = std::move(source);
    ne.stacks = 1;
    effects_.push_back(std::move(ne));
}

void EffectManager::remove(EffectId id) {
    effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [id](const StatusEffect& e){ return e.id == id; }), effects_.end());
}

bool EffectManager::has(EffectId id) const {
    for (auto& e : effects_) if (e.id == id && e.duration != 0) return true;
    return false;
}

int EffectManager::getPower(EffectId id) const {
    for (auto& e : effects_) if (e.id == id) return e.power;
    return 0;
}

void EffectManager::tick() {
    for (auto& e : effects_) {
        if (e.duration > 0) e.duration--;
    }
    effects_.erase(std::remove_if(effects_.begin(), effects_.end(), [](const StatusEffect& e){ return e.duration == 0; }), effects_.end());
}

void EffectManager::clear() {
    effects_.clear();
}

}
