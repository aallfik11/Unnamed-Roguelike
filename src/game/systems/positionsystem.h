#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../entity.h"
#include "../tile.h"
#include "../coordhash.h"
#include <unordered_map>
#include <cstdint>

class PositionSystem
{
    using EntityId = uint16_t;
    // using YMap = std::unordered_multimap<uint16_t, EntityId>;
    // using EntityMap = std::unordered_multimap<uint16_t, std::pair<uint16_t, EntityId>>;
    using EntityMap = std::unordered_multimap<std::weak_ptr<Coordinates>, EntityId>;

    using CoordMap = std::unordered_map<EntityId, std::weak_ptr<Coordinates>>;

    using GameMap = std::vector<std::vector<Tile>>;

    EntityMap CoordsWithEntities;
    CoordMap EntitiesWithCoords;
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

public:
    PositionSystem(GameMap &given_map) : map_{given_map}
    {
    }

    /**
     * @brief
     *
     * @brief adds a weak_ptr of type Entity to a tile at a given position
     * @param x - coordinate x
     * @param y - coordinate y
     * @return true - entity successfully added
     * @return false - entity could not be placed at given coordinates
     */

    bool addEntityToMap(std::shared_ptr<Entity> &entity,
                        uint16_t x,
                        uint16_t y)
    {
        if (checkCoordinateValidity(x, y))
        {
            map_[x][y].entities.emplace_back(entity);
        }
        return true;
    }

    /**
     * @brief
     *
     * @param x - coordinate x
     * @param y - coordinate y
     * @return true - collission
     * @return false - no collision
     */

    bool checkCollision(uint16_t x, uint16_t y)
    {
        return (checkCoordinateValidity(x, y)) ? (map_[x][y].type & TileType::TRAVERSIBLE)
                                               : false;
    }

    bool updatePosition(uint16_t x, uint16_t y)
    {
    }

    std::optional<std::vector<EntityId>> &get_entity_ids(uint16_t x, uint16_t y)
    {
        std::optional<std::vector<EntityId>> potential_ids;
        auto coords = std::weak_ptr<Coordinates>(std::shared_ptr<Coordinates>(new Coordinates(x, y)));
        if (CoordsWithEntities.contains(coords))
        {
            auto range = CoordsWithEntities.equal_range(coords);
            while (range.first != range.second)
            {
                //                                              entity_id
                potential_ids->emplace_back(range.first);
                range.first++;
            }
        }
        return potential_ids;
    }
    std::optional<std::vector<EntityId>> &get_all_entity_ids()
    {
        std::optional<std::vector<EntityId>> potential_ids;
        for (auto &entity_id : CoordsWithEntities)
        {
            potential_ids->emplace_back(entity_id.first);
        }
        return potential_ids;
    }

    std::shared_ptr<Coordinates> &get_entity_coordinates(EntityId entity_id)
    {
        if (EntitiesWithCoords.contains(entity_id))
        {
            return EntitiesWithCoords.at(entity_id).lock();
        }
        return std::shared_ptr<Coordinates>(nullptr);
    }

    void add_entity(EntityId entity_id, std::shared_ptr<Coordinates> &coordinates)
    {
        EntitiesWithCoords.emplace(entity_id,
                                   std::weak_ptr<Coordinates>(coordinates));

        CoordsWithEntities.emplace(coordinates->x,
                                   std::pair<uint16_t, EntityId>(coordinates->y, entity_id));
    }

    // void remove_entity(EntityId entity_id)
    // {
    //     if (EntitiesWithCoords.contains(entity_id))
    //     {

    //             auto range = CoordsWithEntities.equal_range(coord->x);
    //             while (range.first != range.second)
    //             {
    //                 if (range.first->second.second == entity_id)
    //                 {
    //                     CoordsWithEntities.erase(range.first);
    //                     break;
    //                 }
    //                 range.first++;
    //             }

    //         EntitiesWithCoords.erase(entity_id);
    //     }
    // }

    // void remove_entities(std::vector<EntityId> &entity_ids)
    // {
    //     for (auto entity_id : entity_ids)
    //     {
    //         if (EntitiesWithCoords.contains(entity_id))
    //         {
    //             if (auto coord = EntitiesWithCoords.at(entity_id).lock())
    //             {
    //                 auto range = CoordsWithEntities.equal_range(coord->x);
    //                 while (range.first != range.second)
    //                 {
    //                     if (range.first->second.second == entity_id)
    //                     {
    //                         CoordsWithEntities.erase(range.first);
    //                         break;
    //                     }
    //                     range.first++;
    //                 }
    //             }
    //             EntitiesWithCoords.erase(entity_id);
    //         }
    //     }
    // }

    void modify_position(EntityId entity_id, uint16_t new_x, uint16_t new_y)
    {
        if (EntitiesWithCoords.contains(entity_id))
        {
            if (auto coord = EntitiesWithCoords.at(entity_id).lock())
            {
            }
        }
    }
};

#endif /*POSITIONSYSTEM*/