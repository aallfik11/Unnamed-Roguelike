#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../components/tilecomponent.h"
#include "../coordhash.h"
#include "../directions.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include "../tile.h"
#include <any>
#include <cstdint>
#include <list>
#include <unordered_set>

class PositionSystem : public System, public EntityHolder
{
    using EntityId  = uint32_t;

    using GameMap   = std::vector<std::vector<Tile>>;
    using Message =
        std::tuple<SystemAction::POSITION, Entity *, uint16_t, uint16_t>;

    std::unordered_set<Entity *> entity_positions_;
    std::list<Message>           messages_;
    GameMap                     &map_;

    /**
     * @brief Checks if the coordinates are within the map's bounds
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Coordinates are valid,
     * @return false - Coordinates are not valid
     */
    bool checkCoordinateValidity(uint16_t x, uint16_t y)
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
    bool checkCollision(uint16_t x, uint16_t y)
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
    bool updatePosition(Entity *const entity, uint16_t x, uint16_t y)
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
    bool updatePosition(Entity *const entity, Direction direction)
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

    std::list<Entity *> getEntitiesAtCoordinates(uint16_t x, uint16_t y)
    {
        std::list<Entity *> entitiesAtCoordinates;
        for (auto &entity : entity_positions_)
        {
            if (auto coords_ptr = entity->getComponent<Coordinates>())
            {
                if (coords_ptr->x == x && coords_ptr->y == y)
                {
                    entitiesAtCoordinates.emplace_back(entity);
                }
            }
        }
        return entitiesAtCoordinates;
    }

    /**
     * @brief Adds an entity to the system
     *
     * @param entity
     */
    void addEntity(Entity *const entity) { entity_positions_.emplace(entity); }

    /**
     * @brief Deletes entity with a given id, does nothing if no id is matched
     * to the argument
     *
     * @param entity
     */
    void deleteEntity(Entity *const entity) { entity_positions_.erase(entity); }

    void updateData() override
    {
        for (auto &message : messages_)
        {

            auto x = std::get<2>(message);
            auto y = std::get<3>(message);
            if (std::get<0>(message) == SystemAction::POSITION::UPDATE)
            {
                auto entity = std::get<1>(message);
                updatePosition(entity, x, y);
            }
            else
            {
                getEntitiesAtCoordinates(x, y);
            }
        }
    }

    void readSystemMessages()
        override // todo: change to make this a parallel listener;
    {
        for (auto &message : (*system_messages_)[SystemType::POSITION])
        {
            auto message_iterator = message.begin();

            // true -> update position
            // false -> get entities
            auto update_or_get =
                std::any_cast<SystemAction::POSITION>(*message_iterator);
            auto entity = std::any_cast<Entity *>(*(message_iterator + 1));
            auto x      = std::any_cast<uint16_t>(*(message_iterator + 2));
            auto y      = std::any_cast<uint16_t>(*(message_iterator + 3));
            messages_.emplace_back(update_or_get, entity, x, y);
        }
    }

    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::POSITION].clear();
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
        std::shared_ptr<std::list<uint32_t>> entity_requests(
            new std::list<uint32_t>);
        uint32_t temp{};
        for (std::size_t i = 0; i < positions_size; i++)
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

#endif /*POSITIONSYSTEM*/
