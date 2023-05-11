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
#include "../entityholder.h"
#include "../system.h"
#include "healthsystem.h"
#include <istream>
#include <list>
#include <ostream>
#include <unordered_set>

class EffectSystem : public System, public EntityHolder
{

    std::unordered_set<Entity *> buffable_entities_;

public:
    void addEntity(Entity *const entity) { buffable_entities_.insert(entity); }
    void addEntity(std::initializer_list<Entity *> &entities)
    {
        for (auto &entity : entities)
        {
            buffable_entities_.insert(entity);
        }
    }
    void applyEffect(Entity *const caller, EffectComponent *const effect_ptr)
    {
        using SystemAction::HEALTH::CURRENT;
        using SystemAction::HEALTH::DAMAGE;
        using SystemAction::HEALTH::HEAL;
        using SystemAction::HEALTH::MAX;

        if ((effect_ptr->effect & Effect::HEAL) != Effect::NONE)
        {
            // HealthSystem::updateHealth(
            //     caller, effect_ptr->effect_strength * 5, HEAL | CURRENT);

            auto message = {
                std::make_any<SystemAction::HEALTH>(
                    SystemAction::HEALTH::UPDATE),
                std::make_any<Entity *>(caller),
                std::make_any<uint16_t>(effect_ptr->effect_strength * 5),
                std::make_any<SystemAction::HEALTH>(HEAL | CURRENT)};

            // (*system_messages_)[SystemType::HEALTH].emplace_back(message);
            sendSystemMessage(SystemType::HEALTH, message);
            if ((effect_ptr->effect & Effect::APPLY_ONCE) != Effect::NONE)
            {
                effect_ptr->effect |= Effect::APPLIED;
            }
            return;
        }

        if ((effect_ptr->effect & Effect::POISON) != Effect::NONE)
        {
            // HealthSystem::updateHealth(
            //     caller, effect_ptr->effect_strength, DAMAGE | CURRENT);

            auto message = {
                std::make_any<SystemAction::HEALTH>(
                    SystemAction::HEALTH::UPDATE),
                std::make_any<Entity *>(caller),
                std::make_any<uint16_t>(effect_ptr->effect_strength),
                std::make_any<SystemAction::HEALTH>(DAMAGE | CURRENT)};

            // (*system_messages_)[SystemType::HEALTH].emplace_back(message);
            sendSystemMessage(SystemType::HEALTH, message);
            return;
        }

        if ((effect_ptr->effect & Effect::BLEED) != Effect::NONE)
        {
            auto message = {
                std::make_any<SystemAction::HEALTH>(
                    SystemAction::HEALTH::UPDATE),
                std::make_any<uint16_t>(effect_ptr->effect_strength * 2),
                std::make_any<SystemAction::HEALTH>(DAMAGE | CURRENT)};

            // (*system_messages_)[SystemType::HEALTH].emplace_back(message);
            sendSystemMessage(SystemType::HEALTH, message);
            return;
        }
        if ((effect_ptr->effect & Effect::IRONSKIN) != Effect::NONE)
        {
            effect_ptr->effect |= Effect::APPLIED;
            return;
        }

        if ((effect_ptr->effect & Effect::BLIND) != Effect::NONE)
        {
            effect_ptr->effect |= Effect::APPLIED;
            return;
        }

        if ((effect_ptr->effect & Effect::STRENGTH) != Effect::NONE)
        {
            effect_ptr->effect |= Effect::APPLIED;
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

                if ((buff_iterator->second->effect & Effect::APPLIED) ==
                    Effect::NONE)
                {
                    applyEffect(entity, buff_iterator->second.get());
                }
                if ((buff_iterator->first & Effect::PERMANENT) == Effect::NONE)
                {
                    buff_iterator->second->effect_duration -= 1;
                }
            }

            for (auto &effect : to_be_removed)
            {
                buffs_ptr->buffs.erase(effect);
            }
        }
    }
    void updateEffects(Entity *const entity)
    {
        if (auto buffs_ptr = entity->getComponent<BuffComponent>())
        {
            for (auto &buff : buffs_ptr->buffs)
            {
                buff.second->effect_duration -= 1;
                if ((buff.second->effect & Effect::APPLIED) == Effect::NONE)
                {
                    applyEffect(entity, buff.second.get());
                }
            }
        }
    }
    void addEffects(Entity *const entity, const BuffComponent *const buffs_ptr)
    {
        if (buffable_entities_.contains(entity) == false)
        {
            addEntity(entity);
        }

        if (entity->hasComponent<BuffComponent>() == false)
        {
            entity->addComponent(buffs_ptr->clone());
            return;
        }

        auto entity_buffs = entity->getComponent<BuffComponent>();
        for (auto &buff : buffs_ptr->buffs)
        {
            entity_buffs->buffs[buff.first] =
                castToComponent<EffectComponent>(buff.second->clone());
        }
    }

    void cleanseEffects(Entity *const              entity,
                        const BuffComponent *const effects)
    {
        if (effects == nullptr)
        {
            return cleanseEffects(entity);
        }

        if (auto entity_buffs = entity->getComponent<BuffComponent>())
        {
            if (buffable_entities_.contains(entity) == false)
            {
                buffable_entities_.insert(entity);
            }

            for (auto &effect : effects->buffs)
            {
                entity_buffs->buffs.erase(effect.first);
            }
        }
    }
    void cleanseEffects(Entity *const entity)
    {
        if (auto entity_buffs = entity->getComponent<BuffComponent>())
        {
            if (buffable_entities_.contains(entity) == false)
            {
                buffable_entities_.insert(entity);
            }

            for (auto buff_iterator = entity_buffs->buffs.begin();
                 buff_iterator != entity_buffs->buffs.end();)
            {
                if ((buff_iterator->first & Effect::PERMANENT) == Effect::NONE)
                {
                    buff_iterator = entity_buffs->buffs.erase(buff_iterator);
                }
                else
                    buff_iterator++;
            }
        }
    }
    void updateData() override { updateEffects(); }
    void readSystemMessages() override
    {
        for (auto &message : (*system_messages_)[SystemType::EFFECT])
        {
            auto message_iterator = message.begin();
            auto action =
                std::any_cast<SystemAction::EFFECT>(*message_iterator);
            auto entity = std::any_cast<Entity *>(*(message_iterator + 1));
            auto argument_iterator = message_iterator + 2;
            switch (action)
            {
            case SystemAction::EFFECT::ADD:
            {
                auto buffs = std::any_cast<BuffComponent *>(*argument_iterator);
                addEffects(entity, buffs);
                break;
            }
            case SystemAction::EFFECT::APPLY:
            {
                auto effect =
                    std::any_cast<EffectComponent *>(*argument_iterator);
                applyEffect(entity, effect);
                break;
            }
            case SystemAction::EFFECT::CLEANSE:
            {
                auto buffs = std::any_cast<BuffComponent *>(*argument_iterator);
                cleanseEffects(entity, buffs);
                break;
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::EFFECT].clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::EFFECT << ' ' << buffable_entities_.size() << ' ';
        for (auto &entity : buffable_entities_)
        {
            os << entity->getId() << ' ';
        }
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        std::size_t buffable_entities_size{};
        is >> buffable_entities_size;
        if (buffable_entities_size == 0)
        {
            return is;
        }
        std::shared_ptr<std::list<uint32_t>> entity_requests(
            new std::list<uint32_t>);
        uint32_t temp{};
        for (std::size_t i = 0; i < buffable_entities_size; i++)
        {
            is >> temp;
            entity_requests->push_back(temp);
        }
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<EntityHolder *>(this),
            std::make_any<std::shared_ptr<std::list<uint32_t>>>(
                entity_requests)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        return is;
    }
    void loadEntities(std::shared_ptr<std::list<Entity *>> &entities) override
    {
        for (auto &entity : *entities)
        {
            addEntity(entity);
        }
    }
};

#endif /*EFFECTSYSTEM_H*/
