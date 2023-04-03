#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../components/tilecomponent.h"
#include "../coordhash.h"
#include "../directions.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../tile.h"
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

class PositionSystem
{
    using EntityId       = uint32_t;
    using Coords         = std::pair<uint16_t, uint16_t>;
    using CoordEntityMap = std::unordered_multimap<Coords, EntityId>;
    using GameMap        = std::vector<std::vector<Tile>>;
    using EntityPtr      = std::shared_ptr<Entity>;
    using EntityTileMap =
        std::unordered_map<EntityId, std::weak_ptr<TileComponent>>;
    using EntityCoordMap =
        std::unordered_map<EntityId, std::weak_ptr<Coordinates>>;

    // using EntityPositions = std::unordered_set<EntityPtr>;

    CoordEntityMap                coords_with_entities_;
    EntityTileMap                 entities_with_tiles_;
    EntityCoordMap                entities_with_coords_;
    std::unordered_set<EntityPtr> entity_positions_;
    GameMap                      &map_;

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

        if (!(map_[x][y].type & TileType::TRAVERSIBLE))
            return true;

        if (map_[x][y].type & TileType::HAS_MONSTER)
            return true;

        // for (auto entity : entity_positions_)
        // {
        //     auto coords        = entity->getComponent<Coordinates>();
        //     auto tilecomponent = entity->getComponent<TileComponent>();
        //     if (tilecomponent->tile.type & TileType::TRAVERSIBLE)
        //         continue;
        //     if (coords->x == x && coords->y == y)
        //         return true;
        // }

        return false;
    }

public:
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
    bool updatePosition(const EntityPtr &entity, uint16_t x, uint16_t y)
    {
        if (entity_positions_.contains(entity) == false)
        {
            return false;
        }

        if (checkCollision(x, y))
            return false;

        if (auto position = entity->getComponent<Coordinates>())
        {
            auto type = entity->type;
            if (type & EntityType::CREATURE)
            {
                map_[position->x][position->y].type &= ~TileType::HAS_MONSTER;
                map_[x][y].type                     |= TileType::HAS_MONSTER;
            }

            position->x = x;
            position->y = y;
            return true;
        }
        return false;

        // if (auto coord_ptr = entities_with_coords_.at(entity).lock())
        // {

        //     auto range =
        //     coords_with_entities_.equal_range(Coords(coord_ptr->x,
        //     coord_ptr->y)); while (range.first != range.second)
        //     {
        //         if (range.first->second == entity)
        //         {
        //             coords_with_entities_.erase(range.first);
        //             break;
        //         }

        //         range.first++;
        //     }
        //     coord_ptr->x = x;
        //     coord_ptr->y = y;
        //     coords_with_entities_.emplace(std::make_pair(Coords(x, y),
        //     entity));
        // }
        // else
        // {
        //     deleteEntity(entity);
        //     return false;
        // }
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
    bool updatePosition(const EntityPtr &entity, Direction direction)
    {

        if (entity_positions_.contains(entity) == false)
            return false;

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

        // if (auto coord_ptr = entities_with_coords_.at(entity).lock())
        // {
        //     auto x = coord_ptr->x;
        //     auto y = coord_ptr->y;
        //     switch (direction)
        //     {
        //     case Direction::UP:
        //         return updatePosition(entity, x, y - 1);

        //     case Direction::DOWN:
        //         return updatePosition(entity, x, y + 1);

        //     case Direction::LEFT:
        //         return updatePosition(entity, x - 1, y);

        //     case Direction::RIGHT:
        //         return updatePosition(entity, x + 1, y);
        //     default:
        //         return false;
        //     }
        // }
        // else
        // {
        //     deleteEntity(entity);
        //     return false;
        // }
    }

    /**
     * @brief (DO NOT USE) Gets all entity ids at given coordinates
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return std::vector<EntityId>
     */
    std::vector<EntityId> getEntityIds(uint16_t x, uint16_t y)
    {
        std::vector<EntityId> potential_ids;
        if (coords_with_entities_.contains(Coords(x, y)))
        {
            auto range = coords_with_entities_.equal_range(Coords(x, y));
            while (range.first != range.second)
            {
                potential_ids.emplace_back(range.first->second);
                range.first++;
            }
        }
        return potential_ids;
    }

    /**
     * @brief (DO NOT USE) Gets all entity ids currently stored in the system
     *
     * @return std::optional<std::vector<EntityId>>
     */
    std::vector<EntityId> getAllEntityIds()
    {
        std::vector<EntityId> potential_ids;
        for (auto &entity : entities_with_coords_)
        {
            potential_ids.emplace_back(entity.first);
        }
        return potential_ids;
    }

    /**
     * @brief (DO NOT USE) Gets a given entities' "Coordinates" component
     *
     * @param entity
     * @return std::shared_ptr<Coordinates>
     */
    std::shared_ptr<Coordinates> getEntityCoordinates(EntityId entity)
    {
        if (entities_with_coords_.contains(entity))
        {
            return entities_with_coords_.at(entity).lock();
        }
        return std::shared_ptr<Coordinates>(nullptr);
    }

    /**
     * @brief Adds an entity to the system
     *
     * @param entity
     */
    void addEntity(const EntityPtr &entity)
    {
        entity_positions_.emplace(entity);
        // if (auto coord_ptr = entity->getComponent<Coordinates>())
        // {
        //     entities_with_coords_.emplace(entity->getId(),
        //                                   coord_ptr);

        //     coords_with_entities_.emplace(Coords(coord_ptr->x, coord_ptr->y),
        //                                   entity->getId());
        // }

        // if (auto tile_ptr = entity->getComponent<TileComponent>())
        // {
        //     entities_with_tiles_.emplace(entity->getId(),
        //                                  tile_ptr);
        // }
    }

    /**
     * @brief Deletes entity with a given id, does nothing if no id is matched
     * to the argument
     *
     * @param entity
     */
    void deleteEntity(const EntityPtr &entity)
    {
        entity_positions_.erase(entity);
        // if (entities_with_coords_.contains(entity) == false)
        //     return;

        // if (auto coord_ptr = entities_with_coords_.at(entity).lock())
        // {
        //     auto range =
        //     coords_with_entities_.equal_range(Coords(coord_ptr->x,
        //     coord_ptr->y)); while (range.first != range.second)
        //     {
        //         if (range.first->second == entity)
        //         {
        //             coords_with_entities_.erase(range.first);
        //             break;
        //         }
        //         range.first++;
        //     }
        //     entities_with_coords_.erase(entity);
        // }

        // if (entities_with_tiles_.contains(entity))
        // {
        //     entities_with_tiles_.erase(entity);
        // }
    }
};

#endif /*POSITIONSYSTEM*/