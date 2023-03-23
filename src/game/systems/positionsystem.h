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
    using EntityId = uint16_t;
    using Coords = std::pair<uint16_t, uint16_t>;
    using CoordEntityMap = std::unordered_multimap<Coords, EntityId>;
    using EntityTileMap = std::unordered_map<EntityId, std::weak_ptr<TileComponent>>;
    using EntityCoordMap = std::unordered_map<EntityId, std::weak_ptr<Coordinates>>;
    using GameMap = std::vector<std::vector<Tile>>;

    CoordEntityMap coords_with_entities_;
    EntityTileMap entities_with_tiles_;
    EntityCoordMap entities_with_coords_;
    GameMap &map_;

    /**
     * @brief checks if the coordinates are within the map's bounds
     *
     * @param x - coordinate x
     * @param y - coordinate y
     * @return true - coordinates are valid
     * @return false - coordinates are not valid
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
     * @brief checks for potential collisions with walls and other entities
     *
     * @param x - coordinate x
     * @param y - coordinate y
     * @return true - collission
     * @return false - no collision
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
                        if (tile->type ^ TileType::TRAVERSIBLE)
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
     * @brief updates given entities' position in the system
     *
     * @param entity_id - entity whose position will be updated
     * @param x
     * @param y
     * @return true - successfully updated the position
     * @return false - position could not be updated
     */
    bool updatePosition(EntityId entity_id, uint16_t x, uint16_t y)
    {
        if (entities_with_coords_.contains(entity_id) == false)
            return false;

        if (checkCollision(x, y))
            return false;

        if (auto coord_ptr = entities_with_coords_.at(entity_id).lock())
        {
            coord_ptr->x = x;
            coord_ptr->y = y;

            auto range = coords_with_entities_.equal_range(Coords(coord_ptr->x, coord_ptr->y));
            while (range.first != range.second)
            {
                if (range.first->second == entity_id)
                    coords_with_entities_.erase(range.first);

                range.first++;
            }
            coords_with_entities_.emplace(std::make_pair(Coords(x, y), entity_id));
        }
        else
        {
            deleteEntity(entity_id);
            return false;
        }

        return true;
    }

    std::optional<std::vector<EntityId>> &getEntityIds(uint16_t x, uint16_t y)
    {
        std::optional<std::vector<EntityId>> potential_ids;
        if (coords_with_entities_.contains(Coords(x, y)))
        {
            auto range = coords_with_entities_.equal_range(Coords(x, y));
            while (range.first != range.second)
            {
                potential_ids->emplace_back(range.first->second);
                range.first++;
            }
        }
        return potential_ids;
    }

    std::optional<std::vector<EntityId>> &getAllEntityIds()
    {
        std::optional<std::vector<EntityId>> potential_ids;
        for (auto &entity_id : entities_with_coords_)
        {
            potential_ids->emplace_back(entity_id.first);
        }
        return potential_ids;
    }

    std::shared_ptr<Coordinates> getEntityCoordinates(EntityId entity_id)
    {
        if (entities_with_coords_.contains(entity_id))
        {
            return entities_with_coords_.at(entity_id).lock();
        }
        return std::shared_ptr<Coordinates>(nullptr);
    }

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
        }

        if (entities_with_tiles_.contains(entity_id))
        {
            entities_with_tiles_.erase(entity_id);
        }
    }

    // void remove_entity(EntityId entity_id)
    // {
    //     if (entities_with_coords_.contains(entity_id))
    //     {

    //             auto range = coords_with_entities_.equal_range(coord->x);
    //             while (range.first != range.second)
    //             {
    //                 if (range.first->second.second == entity_id)
    //                 {
    //                     coords_with_entities_.erase(range.first);
    //                     break;
    //                 }
    //                 range.first++;
    //             }

    //         entities_with_coords_.erase(entity_id);
    //     }
    // }

    // void remove_entities(std::vector<EntityId> &entity_ids)
    // {
    //     for (auto entity_id : entity_ids)
    //     {
    //         if (entities_with_coords_.contains(entity_id))
    //         {
    //             if (auto coord = entities_with_coords_.at(entity_id).lock())
    //             {
    //                 auto range = coords_with_entities_.equal_range(coord->x);
    //                 while (range.first != range.second)
    //                 {
    //                     if (range.first->second.second == entity_id)
    //                     {
    //                         coords_with_entities_.erase(range.first);
    //                         break;
    //                     }
    //                     range.first++;
    //                 }
    //             }
    //             entities_with_coords_.erase(entity_id);
    //         }
    //     }
    // }
};

#endif /*POSITIONSYSTEM*/