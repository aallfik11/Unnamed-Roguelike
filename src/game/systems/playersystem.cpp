#include "playersystem.h"
#include "../components/buffcomponent.h"
#include "../components/coordinates.h"
#include "../components/effectcomponent.h"
#include "../components/tilecomponent.h"
#include "../components/health.h"
#include "../components/hungercomponent.h"
#include "../components/lineofsightcomponent.h"
#include "../directions.h"
#include "../effect.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../itemtypes.h"
#include "../rarity.h"
#include "../system.h"
#include "positionsystem.h"
#include <cmath>
#include <cstdint>
#include <exception>
#include <istream>
#include <list>
#include <memory>

PlayerSystem::PlayerSystem(bool &next_level) : next_level_{next_level}
{
    player_                = nullptr;
    pos_sys_               = nullptr;
    next_movement_         = Direction::NONE;
    last_hit_enemy_        = nullptr;
    last_hit_entity_timer_ = 0;
}

PlayerSystem::PlayerSystem(const observer_ptr<Entity>         player,
                           const observer_ptr<PositionSystem> pos_sys,
                           bool                              &next_level)
    : PlayerSystem(next_level)
{
    player_  = player;
    pos_sys_ = pos_sys;
}

void PlayerSystem::assignPlayer(const observer_ptr<Entity> player)
{
    player_ = player;
}

void PlayerSystem::assignPositionSystem(
    const observer_ptr<PositionSystem> pos_sys)
{
    pos_sys_ = pos_sys;
}

void PlayerSystem::deduceHunger() noexcept(false)
{
    auto hunger_buff = std::make_unique<BuffComponent>();
    if (player_ == nullptr)
        throw std::runtime_error("PlayerSystem: ERROR -> Player unassigned");
    auto hunger_component = player_->getComponent<HungerComponent>();
    if (hunger_component->hunger == 0)
    {
        System::sendSystemMessage(SystemType::HEALTH,
                                  {std::make_any<observer_ptr<Entity>>(player_),
                                   std::make_any<uint16_t>(1),
                                   std::make_any<SystemAction::HEALTH>(
                                       SystemAction::HEALTH::DAMAGE |
                                       SystemAction::HEALTH::CURRENT)});
    }
    if (hunger_component->hunger > 0)
    {
        hunger_component->hunger -= 1;
    }
}

EntityType PlayerSystem::checkMovementDestination(
    const std::list<observer_ptr<Entity>> &entities_at_direction)
{
    if (entities_at_direction.empty())
        return EntityType::NONE;

    for (auto &entity : entities_at_direction)
    {
        if ((entity->type & EntityType::CREATURE) != EntityType::NONE &&
            (entity->type & EntityType::KILLED) == EntityType::NONE)
        {
            last_hit_enemy_        = entity;
            last_hit_entity_timer_ = 5;
            return EntityType::CREATURE;
        }
    }
    return EntityType::ITEM;
}

observer_ptr<Entity> PlayerSystem::getLastDamagedEnemy()
{
    return last_hit_enemy_;
}

void PlayerSystem::updateData() noexcept(false)
{
    if (player_ == nullptr)
        throw std::runtime_error("PlayerSystem: ERROR -> Player unassigned");
    if (pos_sys_ == nullptr)
        throw std::runtime_error(
            "PlayerSystem: ERROR -> Position System unassigned");
    deduceHunger();
    EntityType entity_type_at_destination = EntityType::NONE;
    std::list<observer_ptr<Entity>> entities_at_destination;
    auto coordinates = player_->getComponent<Coordinates>();
    auto current_x   = coordinates->x;
    auto current_y   = coordinates->y;
    auto next_x      = current_x;
    auto next_y      = current_y;
    switch (next_movement_)
    {
    case Direction::UP:
    {
        next_y = current_y - 1;
        entities_at_destination =
            pos_sys_->getEntitiesAtCoordinates(current_x, next_y);
        entity_type_at_destination =
            checkMovementDestination(entities_at_destination);
        break;
    }
    case Direction::DOWN:
    {
        next_y = current_y + 1;
        entities_at_destination =
            pos_sys_->getEntitiesAtCoordinates(current_x, next_y);
        entity_type_at_destination =
            checkMovementDestination(entities_at_destination);
        break;
    }
    case Direction::LEFT:
    {
        next_x = current_x - 1;
        entities_at_destination =
            pos_sys_->getEntitiesAtCoordinates(next_x, current_y);
        entity_type_at_destination =
            checkMovementDestination(entities_at_destination);
        break;
    }
    case Direction::RIGHT:
    {
        next_x = current_x + 1;
        entities_at_destination =
            pos_sys_->getEntitiesAtCoordinates(next_x, current_y);
        entity_type_at_destination =
            checkMovementDestination(entities_at_destination);
        break;
    }
    case Direction::NONE:
    {
        return;
    }
    }
    switch (entity_type_at_destination)
    {
    case EntityType::CREATURE:
    {
        System::sendSystemMessage(
            SystemType::ATTACK,
            {std::make_any<observer_ptr<Entity>>(player_),
             std::make_any<observer_ptr<Entity>>(last_hit_enemy_)});
        break;
    }
    case EntityType::ITEM:
    {
        System::sendSystemMessage(
            SystemType::INVENTORY,
            {std::make_any<SystemAction::INVENTORY>(
                 SystemAction::INVENTORY::ADD),
             std::make_any<observer_ptr<Entity>>(player_),
             std::make_any<std::list<observer_ptr<Entity>>>(
                 entities_at_destination)});
        for (auto &item : entities_at_destination)
        {
            if (item->getComponent<TileComponent>()->sprite ==
                TileAppearance::GOAL_ITEM) [[unlikely]]
            {
                next_level_ = true;
            }
            System::sendSystemMessage(
                SystemType::POSITION,
                {std::make_any<SystemAction::POSITION>(
                     SystemAction::POSITION::DELETE),
                 std::make_any<observer_ptr<Entity>>(item)});
        }
        [[fallthrough]];
    }
    case EntityType::NONE:
    {
        if ((pos_sys_->getTileAtCoordinates(next_x, next_y).type &
             TileType::HAS_STAIRS) != TileType::NONE)
        {
            next_level_ = true;
        }
        System::sendSystemMessage(SystemType::POSITION,
                                  {std::make_any<SystemAction::POSITION>(
                                       SystemAction::POSITION::UPDATE),
                                   std::make_any<observer_ptr<Entity>>(player_),
                                   std::make_any<uint16_t>(next_x),
                                   std::make_any<uint16_t>(next_y)});
        break;
    }
    default:
        break;
    }
}
void PlayerSystem::readSystemMessages()
{
    for (auto &message : (*System::system_messages_)[SystemType::PLAYER])
    {
        auto message_it   = message.begin();
        auto message_type = std::any_cast<SystemAction::PLAYER>(*message_it);
        ++message_it;
        switch (message_type)
        {
        case SystemAction::PLAYER::MOVE:
        {
            next_movement_ = std::any_cast<Direction>(*message_it);
            break;
        }
        }
    }
}
void PlayerSystem::clearSystemMessages()
{
    (*System::system_messages_)[SystemType::PLAYER].clear();
    next_movement_ = Direction::NONE;
    if (last_hit_enemy_ != nullptr)
    {
        if (last_hit_enemy_->getComponent<LOSComponent>()->has_LOS_to_player ==
            false)
        {
            last_hit_enemy_        = nullptr;
            last_hit_entity_timer_ = 0;
            return;
        }
        if (last_hit_enemy_->getComponent<Health>()->alive == false)
        {
            last_hit_enemy_ = nullptr;
        }
        else if (last_hit_entity_timer_ == 0)
        {
            last_hit_enemy_ = nullptr;
        }
        else
            --last_hit_entity_timer_;
    }
}

void PlayerSystem::resetSystem()
{
    next_movement_         = Direction::NONE;
    last_hit_enemy_        = nullptr;
    last_hit_entity_timer_ = 0;
    player_                = nullptr;
    pos_sys_               = nullptr;
    clearSystemMessages();
}

std::istream &PlayerSystem::deserialize(std::istream &is)
{
    sendSystemMessage(
        SystemType::ENTITY,
        {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
         std::make_any<observer_ptr<EntityHolder>>(this),
         std::make_any<std::list<uint32_t>>(std::list<uint32_t>({1}))});
    return is;
}
void PlayerSystem::loadEntities(std::list<observer_ptr<Entity>> player)
{
    this->player_ = player.front();
}
