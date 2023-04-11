#ifndef EFFECTSYSTEM_H
#define EFFECYSYSTEM_H
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/effectcomponent.h"
#include "../components/health.h"
#include "../components/hungercomponent.h"
#include "../components/weaponslot.h"
#include "../effect.h"
#include "../entity.h"
#include "healthsystem.h"
#include <unordered_set>

class EffectSystem
{
    using BuffPtr   = std::shared_ptr<BuffComponent>;
    using EffectPtr = std::shared_ptr<EffectComponent>;
    using EntityPtr = std::shared_ptr<Entity>;
    std::unordered_set<EntityPtr> buffable_entities_;

public:
    void addEntity(EntityPtr &entity) { buffable_entities_.emplace(entity); }
    void addEntity(std::initializer_list<EntityPtr> &entities)
    {
        for (auto &entity : entities)
        {
            buffable_entities_.emplace(entity);
        }
    }
    void applyEffect(const EntityPtr &caller, EffectPtr &effect_ptr)
    {
        switch (effect_ptr->effect)
        {
        case HEAL:
            HealthSystem::updateHealth(
                caller, effect_ptr->effect_strength * 5, ADD | CURRENT);
            if (effect_ptr->effect & APPLY_ONCE)
            {
                effect_ptr->effect |= APPLIED;
            }
            break;

        case POISON:
            HealthSystem::updateHealth(caller,
                                       effect_ptr->effect_strength,
                                       DEDUCE | CURRENT,
                                       /*ignore_armor*/ true);
            break;
        case BLEED:
            HealthSystem::updateHealth(caller,
                                       effect_ptr->effect_strength * 2,
                                       DEDUCE | CURRENT,
                                       /*ignore_armor*/ true);
            break;
        case IRONSKIN:
            effect_ptr->effect |= Effect::APPLIED;
            break;
        case BLIND:
            /*implementation goes here :^ )*/
            break;
        }
    }
    void updateEffects()
    {
        for (auto &entity : buffable_entities_)
        {
            auto buffs_ptr = entity->getComponent<BuffComponent>();
            for (auto &buff : buffs_ptr->buffs)
            {
                buff.second->effect_duration -= 1;
                if ((buff.second->effect & APPLIED) == false)
                {
                    applyEffect(entity, buff.second);
                }
            }
        }
    }
    void addEffects(EntityPtr &entity, BuffPtr &buffs_ptr)
    {
        if (buffable_entities_.contains(entity) == false)
        {
            addEntity(entity);
        }

        auto entity_buffs = entity->getComponent<BuffComponent>();
        for (auto &buff : buffs_ptr->buffs)
        {
            entity_buffs->buffs[buff.second->effect] = buff.second;
        }
    }
    void cleanseEffect(EntityPtr &entity, Effect effect) {}
};

#endif /*EFFECTSYSTEM_H*/