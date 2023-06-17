#ifndef PLAYERCONTROLSYSTEM_H
#define PLAYERCONTROLSYSTEM_H

#include "../components/coordinates.h"
#include "../components/description.h"
#include "../components/health.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../directions.h"
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

class PlayerControlSystem : public System, public EntityHolder
{
    observer_ptr<Entity>         player_;
    observer_ptr<PositionSystem> pos_sys_;
    Direction                    next_movement_;
    observer_ptr<Entity>         last_hit_enemy_;
    int                          last_hit_entity_timer_;
    bool                        &next_level_;

public:
    PlayerControlSystem(bool &next_level) : next_level_{next_level}
    {
        player_                = nullptr;
        pos_sys_               = nullptr;
        next_movement_         = Direction::NONE;
        last_hit_enemy_        = nullptr;
        last_hit_entity_timer_ = 0;
    }

    PlayerControlSystem(const observer_ptr<Entity>         player,
                        const observer_ptr<PositionSystem> pos_sys,
                        bool                              &next_level)
        : PlayerControlSystem(next_level)
    {
        player_                = player;
        pos_sys_               = pos_sys;
        next_movement_         = Direction::NONE;
        last_hit_entity_timer_ = 0;
    }

    void assignPlayer(const observer_ptr<Entity> player) { player_ = player; }

    void assignPositionSystem(const observer_ptr<PositionSystem> pos_sys)
    {
        pos_sys_ = pos_sys;
    }

    EntityType checkMovementDestination(
        const std::list<observer_ptr<Entity>> &entities_at_direction) noexcept
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

    void updateData() noexcept(false) override
    {
        if (player_ == nullptr)
            throw std::runtime_error(
                "PlayerControlSystem: ERROR -> Player unassigned");
        if (pos_sys_ == nullptr)
            throw std::runtime_error(
                "PlayerControlSystem: ERROR -> Position System unassigned");
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

                System::sendSystemMessage(
                    SystemType::POSITION,
                    {std::make_any<SystemAction::POSITION>(
                         SystemAction::POSITION::DELETE),
                     std::make_any<observer_ptr<Entity>>(item)});
            [[fallthrough]];
        }
        case EntityType::NONE:
        {
            if ((pos_sys_->getTileAtCoordinates(next_x, next_y).type &
                 TileType::HAS_STAIRS) != TileType::NONE)
            {
                next_level_ = true;
            }
            System::sendSystemMessage(
                SystemType::POSITION,
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
    void readSystemMessages() override
    {
        for (auto &message : (*System::system_messages_)[SystemType::PLAYER])
        {
            auto message_it = message.begin();
            auto message_type =
                std::any_cast<SystemAction::PLAYER>(*message_it);
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
    void clearSystemMessages() override
    {
        (*System::system_messages_)[SystemType::PLAYER].clear();
        next_movement_ = Direction::NONE;
        if (last_hit_enemy_ != nullptr)
        {
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

    void resetSystem() override
    {
        next_movement_         = Direction::NONE;
        last_hit_enemy_        = nullptr;
        last_hit_entity_timer_ = 0;
        player_                = nullptr;
        pos_sys_               = nullptr;
        clearSystemMessages();
    }

    std::istream &deserialize(std::istream &is) override
    {
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
             std::make_any<observer_ptr<EntityHolder>>(this),
             std::make_any<std::list<uint32_t>>(std::list<uint32_t>({1}))});
        return is;
    }
    void loadEntities(std::list<observer_ptr<Entity>> player) override
    {
        this->player_ = player.front();
    }
};

#endif /*PLAYERCONTROLSYSTEM_H*/
