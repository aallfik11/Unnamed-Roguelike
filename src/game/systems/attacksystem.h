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
#include "healthsystem.h"
#include <cstdint>
#include <random>

namespace AttackSystem
{
using EntityPtr = std::shared_ptr<Entity>;

std::random_device                     rd;
std::mt19937                           mt_engine(rd());
std::uniform_int_distribution<uint8_t> roll_chance(0, 100);

bool attack(EntityPtr &attacker, EntityPtr &defender)
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
        attacker_weapon->weapon_item->getComponent<WeaponComponent>()->damage;
    }

    auto defender_base_AC = defender_base_armor->armor_class;
    if (defender_armor != nullptr)
    {
        defender_base_AC =
            defender_armor->armor_item->getComponent<ArmorComponent>()
                ->armor_class;
    }

    auto ac_modifier = 0;

    // checking if the attack even hits
    if (attacker_effects->buffs.contains(BLIND))
    {
        ac_modifier += 30;
    }
    if (attacker_effects->buffs.contains(BLIND | PERMANENT))
    {
        ac_modifier += 30;
    }
    if (roll_chance(mt_engine) < (defender_base_AC + ac_modifier))
    {
        return false;
    }

    auto attacker_crit = attacker->getComponent<CritComponent>();
    if (attacker_weapon != nullptr)
    {
        attacker_weapon->weapon_item->getComponent<CritComponent>();
    }

    if (attacker_crit->crit_chance >= roll_chance(mt_engine))
    {
        attacker_base_damage =
            attacker_base_damage * attacker_crit->crit_multiplier;

        if (attacker_crit->crit_effects != nullptr)
        {
            for (auto &effect : attacker_crit->crit_effects->buffs)
            {
                if (defender_effects->buffs.contains(effect.first))
                {
                    if (defender_effects->buffs.at(effect.first)
                            ->effect_strength < effect.second->effect_strength)
                    {
                        defender_effects->buffs[effect.first] =
                            std::shared_ptr<EffectComponent>(
                                effect.second->clone());
                    }
                }
            }
        }
    }

    if (attacker_effects->buffs.contains(STRENGTH))
    {
        attacker_base_damage +=
            attacker_effects->buffs[STRENGTH | PERMANENT]->effect_strength;
    }
    if (attacker_effects->buffs.contains((STRENGTH | PERMANENT)))
    {
        attacker_base_damage +=
            attacker_effects->buffs[STRENGTH | PERMANENT]->effect_strength;
    }

    ac_modifier /= 3;

    if (defender_effects->buffs.contains(IRONSKIN))
    {
        ac_modifier += defender_effects->buffs[IRONSKIN]->effect_strength * 5;
    }
    if (defender_effects->buffs.contains(IRONSKIN | PERMANENT))
    {
        ac_modifier +=
            defender_effects->buffs[IRONSKIN | PERMANENT]->effect_strength * 5;
    }

    auto damage =
        (attacker_base_damage * 10) / (defender_base_AC + ac_modifier);
    HealthSystem::updateHealth(defender, damage, DEDUCE | CURRENT);

    return true;
}
} // namespace AttackSystem

#endif /*ATTACKSYSTEM_H*/