#ifndef ATTACKSYSTEM_H
#define ATTACKSYSTEM_H
#include "../components/amuletslot.h"
#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/critcomponent.h"
#include "../components/effectcomponent.h"
#include "../components/health.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../system.h"
#include "healthsystem.h"
#include <cstdint>
#include <istream>
#include <list>
#include <ostream>
#include <random>
#include <ranges>

class AttackSystem : public System
{

    std::random_device                       rd_;
    std::mt19937                             mt_engine_;
    std::uniform_int_distribution<>          roll_chance_;
    std::list<std::pair<Entity *, Entity *>> messages_;

public:
    AttackSystem()
    {
        mt_engine_.seed(rd_());
        roll_chance_ = std::uniform_int_distribution<>(0, 100);
    }

    bool attack(Entity *const attacker, Entity *const defender)
    {

        // move it down later, as if the attack misses there's no point
        // in getting these componnets
        auto attacker_weapon      = attacker->getComponent<WeaponSlot>();
        auto attacker_base_weapon = attacker->getComponent<WeaponComponent>();
        auto attacker_effects     = attacker->getComponent<BuffComponent>();

        auto defender_armor       = defender->getComponent<ArmorSlot>();
        auto defender_base_armor  = defender->getComponent<ArmorComponent>();
        auto defender_effects     = defender->getComponent<BuffComponent>();

        auto attacker_base_damage = attacker_base_weapon->damage;
        if (attacker_weapon != nullptr)
        {
            if (attacker_weapon->weapon_item != nullptr)
            {
                attacker_base_damage = attacker_weapon->weapon_item
                                           ->getComponent<WeaponComponent>()
                                           ->damage;
            }
        }

        auto defender_base_AC = defender_base_armor->armor_class;
        if (defender_armor != nullptr)
        {
            if (defender_armor->armor_item != nullptr)
            {
                defender_base_AC =
                    defender_armor->armor_item->getComponent<ArmorComponent>()
                        ->armor_class;
            }
        }

        auto ac_modifier = 0;

        // checking if the attack even hits
        if (attacker_effects->buffs.contains(Effect::BLIND))
        {
            ac_modifier += 30;
        }
        if (attacker_effects->buffs.contains(Effect::BLIND | Effect::PERMANENT))
        {
            ac_modifier += 30;
        }
        if (roll_chance_(mt_engine_) < (defender_base_AC + ac_modifier))
        {
            return false;
        }

        auto attacker_crit = attacker->getComponent<CritComponent>();
        if (attacker_weapon != nullptr)
        {
            if (attacker_weapon->weapon_item != nullptr)
            {
                attacker_crit =
                    attacker_weapon->weapon_item->getComponent<CritComponent>();
            }
        }

        if (attacker_crit->crit_chance >= roll_chance_(mt_engine_))
        {
            attacker_base_damage =
                attacker_base_damage * attacker_crit->crit_multiplier;

            if (attacker_crit->crit_effects != nullptr)
            {
                for (auto &effect : attacker_crit->crit_effects->buffs)
                {
                    if (defender_effects->buffs.contains(effect.first) == false)
                    {
                        continue;
                    }
                    if (defender_effects->buffs[effect.first]->effect_strength >
                        effect.second->effect_strength)
                    {
                        continue;
                    }
                    // defender_effects->buffs[effect.first] =
                    //     effect.second->clone();
                    auto message = {
                        std::make_any<SystemAction::EFFECT>(
                            SystemAction::EFFECT::ADD),
                        std::make_any<Entity *>(defender),
                        std::make_any<BuffComponent *>(defender_effects)};
                    sendSystemMessage(SystemType::EFFECT, message);
                }
            }
        }

        if (attacker_effects->buffs.contains(Effect::STRENGTH))
        {
            attacker_base_damage +=
                attacker_effects->buffs[Effect::STRENGTH | Effect::PERMANENT]
                    ->effect_strength;
        }
        if (attacker_effects->buffs.contains(
                (Effect::STRENGTH | Effect::PERMANENT)))
        {
            attacker_base_damage +=
                attacker_effects->buffs[Effect::STRENGTH | Effect::PERMANENT]
                    ->effect_strength;
        }

        ac_modifier /= 3;

        if (defender_effects->buffs.contains(Effect::IRONSKIN))
        {
            ac_modifier +=
                defender_effects->buffs[Effect::IRONSKIN]->effect_strength * 5;
        }
        if (defender_effects->buffs.contains(Effect::IRONSKIN |
                                             Effect::PERMANENT))
        {
            ac_modifier +=
                defender_effects->buffs[Effect::IRONSKIN | Effect::PERMANENT]
                    ->effect_strength *
                5;
        }

        auto damage =
            (attacker_base_damage * 10) / (defender_base_AC + ac_modifier);

        auto message = {
            std::make_any<Entity *>(defender),
            std::make_any<uint16_t>(damage),
            std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::DAMAGE |
                                                SystemAction::HEALTH::CURRENT)};

        // (*system_messages_)[SystemType::HEALTH].emplace_back(message);
        sendSystemMessage(SystemType::HEALTH, message);

        return true;
    }

    void updateData() override
    {
        for (auto &message : messages_)
        {
            attack(message.first, message.second);
        }
    }
    void readSystemMessages() override
    {
        for (auto &message : (*system_messages_)[SystemType::ATTACK])
        {
            auto message_iterator = message.begin();
            auto attacker         = std::any_cast<Entity *>(*message_iterator);
            auto defender = std::any_cast<Entity *>(*(message_iterator + 1));

            messages_.emplace_back(attacker, defender);
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::ATTACK].clear();
        messages_.clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::ATTACK << ' ';
        return os;
    }
    std::istream &deserialize(std::istream &is) override { return is; }
};

#endif /*ATTACKSYSTEM_H*/
