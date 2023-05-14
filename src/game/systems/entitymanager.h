#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "../component.h"
#include "../entity.h"
#include "../entitydeserializer.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include <any>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

class EntityManager : public System
{
    using EntityHashmap = std::unordered_map<uint32_t, std::unique_ptr<Entity>>;
    using Request =
        std::pair<EntityHolder *, std::shared_ptr<std::list<uint32_t>>>;
    EntityHashmap       entities_;
    std::list<uint32_t> purge_list_;
    std::list<Entity *> add_list_;
    std::list<Request>  request_list_;

public:
    uint32_t createEntity(const EntityType          type,
                          std::vector<Component *> &components)
    {
        auto id       = Entity::getMaxId();
        entities_[id] = std::make_unique<Entity>(type, components);
        return id;
    }

    void addEntity(Entity *entity)
    {
        entities_[entity->getId()] = std::unique_ptr<Entity>(entity);
    }

    Entity *getEntity(const uint32_t entity_id) const
    {
        return entities_.at(entity_id).get();
    }

    EntityHashmap &getAllEntities() { return entities_; }

    void markForDeletion(const uint32_t id) { purge_list_.emplace_back(id); }

    void purge()
    {
        for (auto &entity_id : purge_list_)
        {
            entities_.erase(entity_id);
        }
    }

    void handleRequests()
    {
        for (auto &request : request_list_)
        {
            std::shared_ptr<std::list<Entity *>> entities(
                new std::list<Entity *>);
            for (auto &entity_id : *request.second)
            {
                (*entities).push_back(getEntity(entity_id));
            }
            request.first->loadEntities(entities);
        }
    }

    void updateData() override
    {
        purge();
        for (auto &entity : add_list_)
        {
            addEntity(entity);
        }
        handleRequests();
    }
    void readSystemMessages() override
    {
        for (auto &message : (*System::system_messages_)[SystemType::ENTITY])
        {
            auto                 message_it = message.begin();
            SystemAction::ENTITY action =
                std::any_cast<SystemAction::ENTITY>(*message_it);
            message_it++;
            switch (action)
            {
            case SystemAction::ENTITY::ADD:
            {
                add_list_.push_back(std::any_cast<Entity *>(*message_it));
                break;
            }
            case SystemAction::ENTITY::PURGE:
            {
                purge_list_.push_back(std::any_cast<uint32_t>(*message_it));
            }
            case SystemAction::ENTITY::REQUEST:
            {
                request_list_.emplace_back(std::make_pair(
                    std::any_cast<EntityHolder *>(*message_it),
                    std::any_cast<std::shared_ptr<std::list<uint32_t>>>(
                        *(message_it + 1))));
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*System::system_messages_)[SystemType::ENTITY].clear();
        add_list_.clear();
        purge_list_.clear();
        request_list_.clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::ENTITY << ' ' << entities_.size() << std::endl;
        for (auto &[id, entity] : entities_)
        {
            os << entity.get() << std::endl;
        }
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        std::size_t entity_amount{};
        is >> entity_amount;
        for (std::size_t i = 0; i < entity_amount; i++)
        {
            auto entity = std::make_unique<Entity>(true);
            is >> entity.get();
            entities_[entity->getId()] = std::move(entity);
        }
        is >> entity_amount; // to get rid of an additional newline and move the
                             // stream to the correct read position
        return is;
    }
};

#endif /*ENTITYMANAGER*/
