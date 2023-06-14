#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "../component.h"
#include "../components/inventory.h"
#include "../entity.h"
#include "../entitydeserializer.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include <any>
#include <cstdint>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <ranges>
#include <unordered_map>
#include <vector>

class EntityManager : public System
{
    using EntityHashmap = std::unordered_map<uint32_t, std::unique_ptr<Entity>>;
    using Request = std::pair<observer_ptr<EntityHolder>, std::list<uint32_t>>;
    EntityHashmap                   entities_;
    std::list<uint32_t>             purge_list_;
    std::list<observer_ptr<Entity>> add_list_;
    std::list<Request>              request_list_;

public:
    uint32_t createEntity(const EntityType         type,
                          std::vector<Component *> components)
    {
        auto id       = Entity::getMaxId();
        entities_[id] = std::make_unique<Entity>(type, components);
        return id;
    }

    void addEntity(observer_ptr<Entity> entity)
    {
        entities_[entity->getId()] = std::unique_ptr<Entity>(entity);
    }

    observer_ptr<Entity> getEntity(const uint32_t entity_id) const
    {
        return entities_.at(entity_id).get();
    }

    EntityHashmap &getAllEntities() { return entities_; }

    void markForDeletion(const uint32_t id) { purge_list_.emplace_back(id); }
    void unmarkForDeletion(const uint32_t id)
    {
        auto iterator = std::find(purge_list_.begin(), purge_list_.end(), id);
        if (iterator != purge_list_.end())
            purge_list_.erase(iterator);
    }

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
            std::list<observer_ptr<Entity>> entities;
            for (auto &entity_id : request.second)
            {
                entities.push_back(getEntity(entity_id));
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
                add_list_.push_back(
                    std::any_cast<observer_ptr<Entity>>(*message_it));
                break;
            }
            case SystemAction::ENTITY::PURGE:
            {
                purge_list_.push_back(std::any_cast<uint32_t>(*message_it));
                break;
            }
            case SystemAction::ENTITY::REQUEST:
            {
                request_list_.emplace_back(std::make_pair(
                    std::any_cast<observer_ptr<EntityHolder>>(*message_it),
                    std::any_cast<std::list<uint32_t>>(*(message_it + 1))));
                break;
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

    void resetSystem() override
    {
        readSystemMessages();
        updateData();
        clearSystemMessages();
        if (entities_.empty())
            return;
        std::list<std::unique_ptr<Entity>> player_and_items;
        player_and_items.emplace_back(entities_[1].release());
        auto player_inventory =
            entities_[1]->getComponent<Inventory>()->inventory;
        std::list<uint32_t> item_ids;
        for (auto &item : player_inventory)
        {
            player_and_items.emplace_back(entities_[item->getId()].release());
        }

        entities_.clear();
        Entity::resetMaxId();
        for (auto &entity : player_and_items)
        {
            auto ent                = std::make_unique<Entity>(*entity);
            entities_[ent->getId()] = std::move(entity);
        }
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::ENTITY << ' ' << entities_.size() << std::endl;
        for (auto &[id, entity] : entities_)
        {
            os << *entity << std::endl;
        }
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        std::size_t entity_amount{};
        is >> entity_amount;
        entities_.clear();
        entities_.reserve(entity_amount);
        for (std::size_t i = 0; i < entity_amount; i++)
        {
            auto entity = std::make_unique<Entity>(true);
            is >> *entity;
            entities_[entity->getId()] = std::move(entity);
        }
        // is >> entity_amount; // to get rid of an additional newline and move
        // the
        //                      // stream to the correct read position
        return is;
    }

    // DEBUG
    bool operator==(const EntityManager &e)
    {
        for (auto &[key, entity] : this->entities_)
        {
            if (e.entities_.contains(key) == false)
            {
                std::cerr << "KEY MISSING: " << key << ' ';
                return false;
            }
            if (*(this->entities_.at(key)) != *(e.entities_.at(key)))
            {
                std::cerr << "ENTITY MISMATCH: " << key << ' ';
                return false;
            }
        }
        return true;
    }
};

#endif /*ENTITYMANAGER*/
