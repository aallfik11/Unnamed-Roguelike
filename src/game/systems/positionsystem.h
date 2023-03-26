#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../components/tilecomponent.h"
#include "../entity.h"
#include "../tile.h"
#include "../coordhash.h"
#include "../directions.h"
#include <unordered_map>
#include <cstdint>

class PositionSystem
{
    using EntityId = uint32_t;
    using Coords = std::pair<uint16_t, uint16_t>;
    using CoordEntityMap = std::unordered_multimap<Coords, EntityId>;
    using EntityTileMap = std::unordered_map<EntityId, std::weak_ptr<TileComponent>>;
    using EntityCoordMap = std::unordered_map<EntityId, std::weak_ptr<Coordinates>>;
    using GameMap = std::vector<std::vector<Tile>>;

public:
    CoordEntityMap coords_with_entities_;
    EntityTileMap entities_with_tiles_;
    EntityCoordMap entities_with_coords_;
    GameMap &map_;

    /**
     * @brief Checks if the coordinates are within the map's bounds
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Coordinates are valid,
     * @return false - Coordinates are not valid
     */
    inline bool checkCoordinateValidity(uint16_t x, uint16_t y)
    {
        if (map_.empty())
            return false;
        if (x > map_.size())
            return false;
        if (y > map_[x].size())
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

        if (map_[x][y].type & TileType::TRAVERSIBLE)
        {
            if (coords_with_entities_.contains(Coords(x, y)))
            {
                auto range = coords_with_entities_.equal_range(Coords(x, y));
                while (range.first != range.second)
                {
                    if (auto tile = entities_with_tiles_
                                        .at(range.first->second)
                                        .lock())
                    {
                        if ((tile->tile.type & TileType::TRAVERSIBLE) == false)
                            return true;
                    }
                    range.first++;
                }
            }
        }
        else
            return true;

        return false;
    }

public:
    PositionSystem(GameMap &given_map) : map_{given_map}
    {
    }

    /**
     * @brief Updates given entities' position in the system
     *
     * @param entity_id Entity whose position will be updated
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true Successfully updated the position,
     * @return false Position could not be updated
     */
    bool updatePosition(EntityId entity_id, uint16_t x, uint16_t y)
    {
        if (entities_with_coords_.contains(entity_id) == false)
            return false;

        if (checkCollision(x, y))
            return false;

        if (auto coord_ptr = entities_with_coords_.at(entity_id).lock())
        {

            auto range = coords_with_entities_.equal_range(Coords(coord_ptr->x, coord_ptr->y));
            while (range.first != range.second)
            {
                if (range.first->second == entity_id)
                {
                    coords_with_entities_.erase(range.first);
                    break;
                }

                range.first++;
            }
            coord_ptr->x = x;
            coord_ptr->y = y;
            coords_with_entities_.emplace(std::make_pair(Coords(x, y), entity_id));
        }
        else
        {
            deleteEntity(entity_id);
            return false;
        }

        return true;
    }

    /**
     * @brief A version of updatePosition which accepts enumerated type Direction as an argument
     *
     * @param entity_id Entity whose position will be updated
     * @param direction Can be UP, DOWN, LEFT or RIGHT, the position is updated accordingly
     * @return true - Successfuly updated the position,
     * @return false - Position could not be updated
     */
    bool updatePosition(EntityId entity_id, Direction direction)
    {
        if (entities_with_coords_.contains(entity_id) == false)
            return false;

        if (auto coord_ptr = entities_with_coords_.at(entity_id).lock())
        {
            auto x = coord_ptr->x;
            auto y = coord_ptr->y;
            switch (direction)
            {
            case Direction::UP:
                return updatePosition(entity_id, x, y - 1);

            case Direction::DOWN:
                return updatePosition(entity_id, x, y + 1);

            case Direction::LEFT:
                return updatePosition(entity_id, x - 1, y);

            case Direction::RIGHT:
                return updatePosition(entity_id, x + 1, y);
            default:
                return false;
            }
        }
        else
        {
            deleteEntity(entity_id);
            return false;
        }
    }

    /**
     * @brief Gets all entity ids at given coordinates
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
     * @brief Gets all entity ids currently stored in the system
     *
     * @return std::optional<std::vector<EntityId>>
     */
    std::vector<EntityId> getAllEntityIds()
    {
        std::vector<EntityId> potential_ids;
        for (auto &entity_id : entities_with_coords_)
        {
            potential_ids.emplace_back(entity_id.first);
        }
        return potential_ids;
    }

    /**
     * @brief Gets a given entities' "Coordinates" component
     *
     * @param entity_id
     * @return std::shared_ptr<Coordinates>
     */
    std::shared_ptr<Coordinates> getEntityCoordinates(EntityId entity_id)
    {
        if (entities_with_coords_.contains(entity_id))
        {
            return entities_with_coords_.at(entity_id).lock();
        }
        return std::shared_ptr<Coordinates>(nullptr);
    }

    /**
     * @brief Adds an entity to the system
     *
     * @param entity
     */
    void addEntity(std::shared_ptr<Entity> &entity)
    {
        if (auto coord_ptr = entity->getComponent<Coordinates>())
        {
            entities_with_coords_.emplace(entity->getId(),
                                          coord_ptr);

            coords_with_entities_.emplace(Coords(coord_ptr->x, coord_ptr->y),
                                          entity->getId());
        }

        if (auto tile_ptr = entity->getComponent<TileComponent>())
        {
            entities_with_tiles_.emplace(entity->getId(),
                                         tile_ptr);
        }
    }

    /**
     * @brief Deletes entity with a given id, does nothing if no id is matched to the argument
     *
     * @param entity_id
     */
    void deleteEntity(EntityId entity_id)
    {
        if (entities_with_coords_.contains(entity_id) == false)
            return;

        if (auto coord_ptr = entities_with_coords_.at(entity_id).lock())
        {
            auto range = coords_with_entities_.equal_range(Coords(coord_ptr->x, coord_ptr->y));
            while (range.first != range.second)
            {
                if (range.first->second == entity_id)
                {
                    coords_with_entities_.erase(range.first);
                    break;
                }
                range.first++;
            }
            entities_with_coords_.erase(entity_id);
        }

        if (entities_with_tiles_.contains(entity_id))
        {
            entities_with_tiles_.erase(entity_id);
        }
    }
};

#endif /*POSITIONSYSTEM*/