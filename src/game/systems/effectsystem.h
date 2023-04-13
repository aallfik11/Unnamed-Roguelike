#ifndef EFFECTSYSTEM_H
#define EFFECTSYSTEM_H
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/effectcomponent.h"
#include "../components/health.h"
#include "../components/hungercomponent.h"
#include "../components/weaponslot.h"
#include "../effect.h"
#include "../entity.h"
#include "healthsystem.h"
#include <list>
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

        if (effect_ptr->effect & HEAL)
        {
            HealthSystem::updateHealth(
                caller, effect_ptr->effect_strength * 5, ADD | CURRENT);
            if (effect_ptr->effect & APPLY_ONCE)
            {
                effect_ptr->effect |= APPLIED;
            }
            return;
        }

        if (effect_ptr->effect & POISON)
        {
            HealthSystem::updateHealth(caller,
                                       effect_ptr->effect_strength,
                                       DEDUCE | CURRENT,
                                       /*ignore_armor*/ true);
            return;
        }

        if (effect_ptr->effect & BLEED)
        {
            HealthSystem::updateHealth(caller,
                                       effect_ptr->effect_strength * 2,
                                       DEDUCE | CURRENT,
                                       /*ignore_armor*/ true);
            return;
        }
        if (effect_ptr->effect & IRONSKIN)
        {
            effect_ptr->effect |= Effect::APPLIED;
            return;
        }

        if (effect_ptr->effect & BLIND)
        { /*implementation goes here :^ )*/
            return;
        }
    }
    void updateEffects()
    {
        for (auto &entity : buffable_entities_)
        {
            auto buffs_ptr = entity->getComponent<BuffComponent>();
            std::list<BuffComponent::BuffMap::iterator> to_be_removed;
            // for (auto &buff : buffs_ptr->buffs)

            for (auto buff_iterator = buffs_ptr->buffs.begin();
                 buff_iterator != buffs_ptr->buffs.end();
                 buff_iterator++)
            {
                if (buff_iterator->second->effect_duration == 0)
                {
                    to_be_removed.emplace_back(buff_iterator);
                    continue;
                }

                if ((buff_iterator->second->effect & APPLIED) == false)
                {
                    applyEffect(entity, buff_iterator->second);
                }
                buff_iterator->second->effect_duration -= 1;
            }

            for (auto &effect : to_be_removed)
            {
                buffs_ptr->buffs.erase(effect);
            }
        }
    }
    void updateEffects(EntityPtr &entity)
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
    void addEffects(EntityPtr &entity, BuffPtr &buffs_ptr)
    {
        if (buffable_entities_.contains(entity) == false)
        {
            addEntity(entity);
        }

        auto entity_buffs = entity->getComponent<BuffComponent>();
        for (auto &buff : buffs_ptr->buffs)
        {
            entity_buffs->buffs[buff.first] = buff.second;
        }
    }
    void cleanseEffect(EntityPtr &entity, Effect effect)
    {

        if (buffable_entities_.contains(entity) == false)
        {
            return;
        }

        auto entity_buffs = entity->getComponent<BuffComponent>();
        entity_buffs->buffs.erase(effect);
    }
};

#endif /*EFFECTSYSTEM_H*/