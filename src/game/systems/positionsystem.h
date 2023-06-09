#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../components/tilecomponent.h"
#include "../coordhash.h"
#include "../directions.h"
#include "../entity.h"
#include "../entityhash.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include "../tile.h"
#include <any>
#include <cstdint>
#include <list>
#include <mutex>
#include <unordered_set>
#include <utility>

class PositionSystem : public System, public EntityHolder
{
    using EntityId = uint32_t;

    using GameMap  = std::vector<std::vector<Tile>>;
    using Message  = std::tuple<observer_ptr<Entity>, uint16_t, uint16_t>;

    std::unordered_set<observer_ptr<Entity>> entity_positions_;
    std::list<Message>                       pos_change_messages_;
    std::list<std::pair<uint16_t, uint16_t>> request_messages_;
    std::list<observer_ptr<Entity>>          addition_messages_;
    std::list<observer_ptr<Entity>>          removal_messages_;
    std::mutex                               render_mutex;
    GameMap                                 &map_;

    /**
     * @brief Checks if the coordinates are within the map's bounds
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Coordinates are valid,
     * @return false - Coordinates are not valid
     */
    bool checkCoordinateValidity(uint16_t x, uint16_t y) const
    {
        if (map_.empty())
            return false;
        if (x >= map_.size())
            return false;
        if (y >= map_[x].size())
            return false;
        return true;
    }

    /**
     * @brief Checks for potential collisions with walls and other entities
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Collission,
     * @return false - No collision
     */
    bool checkCollision(uint16_t x, uint16_t y) const
    {
        if (checkCoordinateValidity(x, y) == false)
            return true;

        if ((map_[x][y].type & TileType::TRAVERSIBLE) == TileType::NONE)
            return true;

        if ((map_[x][y].type & TileType::HAS_CREATURE) != TileType::NONE)
            return true;

        return false;
    }

public:
    PositionSystem() = delete;
    PositionSystem(GameMap &given_map) : map_{given_map} {}

    /**
     * @brief Updates given entities' position in the system
     *
     * @param entity Entity whose position will be updated
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true Successfully updated the position,
     * @return false Position could not be updated
     */
    bool
    updatePosition(const observer_ptr<Entity> entity, uint16_t x, uint16_t y)
    {
        if (entity_positions_.contains(entity) == false)
        {
            entity_positions_.insert(entity);
        }

        if (checkCollision(x, y))
            return false;

        if (entity->hasComponent<Coordinates>() == false)
            entity->addComponent(new Coordinates(x, y));

        auto position = entity->getComponent<Coordinates>();

        auto type     = entity->type;
        if ((type & (EntityType::CREATURE | EntityType::PLAYER)) !=
            EntityType::NONE)
        {
            map_[position->x][position->y].type &= ~TileType::HAS_CREATURE;
            map_[x][y].type                     |= TileType::HAS_CREATURE;
        }
        else if ((type & (EntityType::CONTAINER | EntityType::ITEM)) !=
                 EntityType::NONE)
        {
            map_[position->x][position->y].type &= ~TileType::HAS_ITEM;
            map_[x][y].type                     |= TileType::HAS_ITEM;
        }

        position->x = x;
        position->y = y;
        return true;
    }

    /**
     * @brief A version of updatePosition which accepts enumerated type
     * Direction as an argument
     *
     * @param entity Entity whose position will be updated
     * @param direction Can be UP, DOWN, LEFT or RIGHT, the position is updated
     * accordingly
     * @return true - Successfuly updated the position,
     * @return false - Position could not be updated
     */
    bool updatePosition(const observer_ptr<Entity> entity, Direction direction)
    {
        if (auto position = entity->getComponent<Coordinates>())
        {
            auto x = position->x;
            auto y = position->y;

            switch (direction)
            {
            case Direction::UP:
                y--;
                break;
            case Direction::DOWN:
                y++;
                break;
            case Direction::LEFT:
                x--;
                break;
            case Direction::RIGHT:
                x++;
                break;
            }

            position->x = x;
            position->y = y;
            return true;
        }
        return false;
    }

    std::list<observer_ptr<Entity>> getEntitiesAtCoordinates(uint16_t x,
                                                             uint16_t y)
    {
        const std::lock_guard<std::mutex> lock(render_mutex);
        std::list<observer_ptr<Entity>>   entities_at_coordinates;
        for (auto &entity : entity_positions_)
        {
            if (auto coords_ptr = entity->getComponent<Coordinates>())
            {
                if (coords_ptr->x == x && coords_ptr->y == y)
                {
                    entities_at_coordinates.emplace_back(entity);
                }
            }
        }
        return entities_at_coordinates;
    }

    /**
     * @brief Adds an entity to the system
     *
     * @param entity
     */
    void addEntity(const observer_ptr<Entity> entity)
    {
        entity_positions_.emplace(entity);
        if (auto entity_coords = entity->getComponent<Coordinates>())
        {
            TileType entity_tile_type = TileType::NONE;
            if ((entity->type & (EntityType::CREATURE | EntityType::PLAYER)) !=
                EntityType::NONE)
                entity_tile_type = TileType::HAS_CREATURE;
            else
                entity_tile_type = TileType::HAS_ITEM;
            map_[entity_coords->x][entity_coords->y].type |= entity_tile_type;
        }
    }

    /**
     * @brief Deletes entity with a given id, does nothing if no id is matched
     * to the argument
     *
     * @param entity
     */
    void deleteEntity(const observer_ptr<Entity> entity)
    {
        auto entity_coords        = entity->getComponent<Coordinates>();

        TileType entity_tile_type = TileType::NONE;
        if ((entity->type & (EntityType::CREATURE | EntityType::PLAYER)) !=
            EntityType::NONE)
            entity_tile_type = TileType::HAS_CREATURE;
        else
            entity_tile_type = TileType::HAS_ITEM;
        map_[entity_coords->x][entity_coords->y].type &= ~entity_tile_type;

        entity_positions_.erase(entity);
    }

    void updateData() override
    {
        for (auto &entity : removal_messages_)
        {
            deleteEntity(entity);
        }
        for (auto &entity : addition_messages_)
        {
            addEntity(entity);
        }
        for (auto &message : pos_change_messages_)
        {
            updatePosition(std::get<0>(message),
                           std::get<1>(message),
                           std::get<2>(message));
        }
    }

    void readSystemMessages()
        override // todo: change to make this a parallel listener;
    {
        for (auto &message : (*system_messages_)[SystemType::POSITION])
        {
            auto message_iterator = message.begin();
            auto message_type =
                std::any_cast<SystemAction::POSITION>(*message_iterator);
            ++message_iterator;
            switch (message_type)
            {
            case SystemAction::POSITION::UPDATE:
            {
                auto entity =
                    std::any_cast<observer_ptr<Entity>>(*message_iterator);
                ++message_iterator;
                auto x = std::any_cast<uint16_t>(*message_iterator);
                ++message_iterator;
                auto y = std::any_cast<uint16_t>(*message_iterator);
                pos_change_messages_.emplace_back(Message(entity, x, y));
                break;
            }
            case SystemAction::POSITION::GET:
            {
                auto x = std::any_cast<uint16_t>(*message_iterator);
                ++message_iterator;
                auto y = std::any_cast<uint16_t>(*message_iterator);
                break;
            }
            case SystemAction::POSITION::ADD:
            {
                auto entity =
                    std::any_cast<observer_ptr<Entity>>(*message_iterator);
                addition_messages_.emplace_back(entity);
                break;
            }
            case SystemAction::POSITION::DELETE:
            {
                auto entity =
                    std::any_cast<observer_ptr<Entity>>(*message_iterator);
                removal_messages_.emplace_back(entity);
                break;
            }
            }
        }
    }

    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::POSITION].clear();
        addition_messages_.clear();
        removal_messages_.clear();
        pos_change_messages_.clear();
        request_messages_.clear();
    }

    virtual std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::POSITION << ' ' << entity_positions_.size() << ' ';
        for (auto &entity : entity_positions_)
        {
            os << entity->getId() << ' ';
        }
        return os;
    }
    virtual std::istream &deserialize(std::istream &is) override
    {
        std::size_t positions_size{};
        is >> positions_size;
        if (positions_size == 0)
        {
            return is;
        }
        entity_positions_.clear();
        entity_positions_.reserve(positions_size);
        std::list<uint32_t> entity_requests;
        uint32_t            temp{};
        for (std::size_t i = 0; i < positions_size; i++)
        {
            is >> temp;
            entity_requests.push_back(temp);
        }
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<observer_ptr<EntityHolder>>(this),
            std::make_any<std::list<uint32_t>>(entity_requests)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        return is;
    }

    void loadEntities(std::list<observer_ptr<Entity>> entities) override
    {
        for (auto &entity : entities)
        {
            addEntity(entity);
        }
    }
};

#endif /*POSITIONSYSTEM*/
