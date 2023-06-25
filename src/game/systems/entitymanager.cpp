#include "entitymanager.h"
#include "../component.h"
#include "../components/amuletslot.h"
#include "../components/armorslot.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/weaponslot.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include <algorithm>
#include <any>
#include <cstdint>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

using EntityHashmap = std::unordered_map<uint32_t, std::unique_ptr<Entity>>;
using Request = std::pair<observer_ptr<EntityHolder>, std::list<uint32_t>>;
uint32_t EntityManager::createEntity(const EntityType         type,
                                     std::vector<Component *> components)
{
    auto id       = Entity::getMaxId();
    entities_[id] = std::make_unique<Entity>(type, components);
    return id;
}

void EntityManager::addEntity(observer_ptr<Entity> entity)
{
    entities_[entity->getId()] = std::unique_ptr<Entity>(entity);
}

observer_ptr<Entity> EntityManager::getEntity(const uint32_t entity_id) const
{
    if (entities_.contains(entity_id))
    {
        return entities_.at(entity_id).get();
    }
    return nullptr;
}

EntityHashmap &EntityManager::getAllEntities() { return entities_; }

void EntityManager::markForDeletion(const uint32_t id)
{
    purge_list_.emplace_back(id);
}
void EntityManager::unmarkForDeletion(const uint32_t id)
{
    auto iterator = std::find(purge_list_.begin(), purge_list_.end(), id);
    if (iterator != purge_list_.end())
        purge_list_.erase(iterator);
}

void EntityManager::purge()
{
    for (auto &entity_id : purge_list_)
    {
        entities_.erase(entity_id);
    }
}

void EntityManager::handleRequests()
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

void EntityManager::updateData()
{
    purge();
    for (auto &entity : add_list_)
    {
        addEntity(entity);
    }
    handleRequests();
}
void EntityManager::readSystemMessages()
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
void EntityManager::clearSystemMessages()
{
    (*System::system_messages_)[SystemType::ENTITY].clear();
    add_list_.clear();
    purge_list_.clear();
    request_list_.clear();
}

void EntityManager::resetSystem()
{
    readSystemMessages();
    updateData();
    clearSystemMessages();
    if (entities_.empty())
        return;
    std::list<std::unique_ptr<Entity>> player_items;
    auto                               player = std::move(entities_[1]);
    auto player_inventory = player->getComponent<Inventory>()->inventory;
    for (auto &item : player_inventory)
    {
        player_items.emplace_back(std::move(entities_[item->getId()]));
    }

    entities_.clear();
    Entity::resetMaxId();

    auto player_weaponslot         = player->getComponent<WeaponSlot>();
    auto player_armorslot          = player->getComponent<ArmorSlot>();
    auto player_amuletslot         = player->getComponent<AmuletSlot>();

    player_weaponslot->weapon_item = nullptr;
    player_armorslot->armor_item   = nullptr;
    player_amuletslot->amulet_slots.clear();

    player->id_  = Entity::max_id_;
    entities_[1] = std::move(player);
    Entity::max_id_++;

    for (auto &item : player_items)
    {
        auto item_component = item->getComponent<ItemComponent>();
        if (item_component->equipped)
        {
            switch (item_component->type)
            {
            case ItemType::ARMOR:
                player_armorslot->armor_item = item.get();
                break;
            case ItemType::WEAPON:
                player_weaponslot->weapon_item = item.get();
                break;
            case ItemType::RING:
                player_amuletslot->amulet_slots.insert(item.get());
                break;
            default:
                break;
            }
        }
        item->id_                  = Entity::max_id_;
        entities_[Entity::max_id_] = std::move(item);
        ++Entity::max_id_;
    }
}

void EntityManager::hardReset() // use carefully
{
    clearSystemMessages();
    entities_.clear();
    Entity::resetMaxId();
}

std::ostream &EntityManager::serialize(std::ostream &os) const
{
    os << SystemType::ENTITY << ' ' << entities_.size() << std::endl;
    for (auto &[id, entity] : entities_)
    {
        os << *entity << std::endl;
    }
    return os;
}

std::istream &EntityManager::deserialize(std::istream &is)
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
    return is;
}

// DEBUG
bool EntityManager::operator==(const EntityManager &e)
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
