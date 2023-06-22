#ifndef ATTACKSYSTEM_H
#define ATTACKSYSTEM_H
#include "../components/amuletslot.h"
#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/critcomponent.h"
#include "../components/effectcomponent.h"
#include "../components/health.h"
#include "../components/name.h"
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

    std::random_device              rd_;
    std::mt19937                    mt_engine_;
    std::uniform_int_distribution<> roll_chance_;
    std::list<std::pair<observer_ptr<Entity>, observer_ptr<Entity>>> messages_;

public:
    AttackSystem()
    {
        mt_engine_.seed(rd_());
        roll_chance_ = std::uniform_int_distribution<>(0, 100);
    }

    inline uint8_t truncateArmorClass(uint8_t ac)
    {
        return (ac > 100) ? 100 : ac;
    }

    bool attack(const observer_ptr<Entity> attacker,
                const observer_ptr<Entity> defender)
    {

        // move it down later, as if the attack misses there's no point
        // in getting these componnets
        auto attacker_weapon      = attacker->getComponent<WeaponSlot>();
        auto attacker_base_weapon = attacker->getComponent<WeaponComponent>();
        auto attacker_effects     = attacker->getComponent<BuffComponent>();

        auto defender_armor       = defender->getComponent<ArmorSlot>();
        auto defender_base_armor  = defender->getComponent<ArmorComponent>();
        auto defender_effects     = defender->getComponent<BuffComponent>();

        bool critical_strike      = false;

        auto crit_roll            = roll_chance_(mt_engine_);

        const std::string miss_message =
            attacker->getComponent<Name>()->name + " missed " +
            defender->getComponent<Name>()->name;

        const std::string hit_message =
            attacker->getComponent<Name>()->name + " hit " +
            defender->getComponent<Name>()->name;

        const std::string crit_message =
            attacker->getComponent<Name>()->name + " critically strikes " +
            defender->getComponent<Name>()->name;

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

        auto combined_ac   = truncateArmorClass(defender_base_AC + ac_modifier);

        auto attacker_crit = attacker->getComponent<CritComponent>();
        if (attacker_weapon != nullptr)
        {
            if (attacker_weapon->weapon_item != nullptr)
            {
                attacker_crit =
                    attacker_weapon->weapon_item->getComponent<CritComponent>();
            }
        }
        if (attacker_crit->crit_chance >= crit_roll)
        {
            critical_strike = true;
            attacker_base_damage =
                attacker_base_damage * attacker_crit->crit_multiplier;
            combined_ac = 1;

            if (attacker_crit->crit_effects != nullptr)
            {
                for (auto &effect : attacker_crit->crit_effects->buffs)
                {
                    if (defender_effects->buffs.contains(effect.first))
                    {
                        if (defender_effects->buffs[effect.first]
                                ->effect_strength >
                            effect.second->effect_strength)
                        {
                            continue;
                        }
                    }
                    // defender_effects->buffs[effect.first] =I'm asking
                    //     effect.second->clone();
                    auto message = {
                        std::make_any<SystemAction::EFFECT>(
                            SystemAction::EFFECT::ADD),
                        std::make_any<observer_ptr<Entity>>(defender),
                        std::make_any<observer_ptr<const BuffComponent>>(
                            attacker_crit->crit_effects.get())};
                    sendSystemMessage(SystemType::EFFECT, message);
                }
            }
        }

        if (roll_chance_(mt_engine_) < combined_ac)
        {
            auto message = miss_message;
            System::sendSystemMessage(SystemType::LOG,
                                      {std::make_any<std::string>(message)});
            return false;
        }

        if (attacker_effects->buffs.contains(Effect::STRENGTH))
        {
            attacker_base_damage +=
                attacker_effects->buffs[Effect::STRENGTH]->effect_strength;
        }
        if (attacker_effects->buffs.contains(
                (Effect::STRENGTH | Effect::PERMANENT)))
        {
            attacker_base_damage +=
                attacker_effects->buffs[Effect::STRENGTH | Effect::PERMANENT]
                    ->effect_strength;
        }

        ac_modifier /=
            3; // blind affects mostly hitrate, it won't impact damage as much

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
        combined_ac  = truncateArmorClass(defender_base_AC + ac_modifier);

        auto damage  = (attacker_base_damage * 10) / (combined_ac);

        auto message = {
            std::make_any<observer_ptr<Entity>>(defender),
            std::make_any<uint16_t>(damage),
            std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::DAMAGE |
                                                SystemAction::HEALTH::CURRENT)};

        auto log = (critical_strike) ? crit_message : hit_message;
        sendSystemMessage(SystemType::LOG, {std::make_any<std::string>(log)});

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
            auto attacker =
                std::any_cast<observer_ptr<Entity>>(*message_iterator);
            auto defender =
                std::any_cast<observer_ptr<Entity>>(*(message_iterator + 1));

            messages_.emplace_back(attacker, defender);
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::ATTACK].clear();
        messages_.clear();
    }
};

#endif /*ATTACKSYSTEM_H*/
