#ifndef LOSSYSTEM_H
#define LOSSYSTEM_H
#include "../components/coordinates.h"
#include "../components/lineofsightcomponent.h"
#include "../entity.h"
#include "../tile.h"
#include <cmath>
#include <memory>
#include <unordered_set>

class LOS_System
{
    using EntityPtr = std::shared_ptr<Entity>;
    using GameMap   = std::vector<std::vector<Tile>>;

    std::unordered_set<EntityPtr> lines_of_sight;
    GameMap                      &map;

    bool lineOfSightAlg(uint16_t init_x,
                        uint16_t init_y,
                        uint16_t target_x,
                        uint16_t target_y,
                        uint16_t seeing_distance)
    {
        uint16_t distance_x = std::abs(init_x - target_x);
        uint16_t distance_y = std::abs(init_y - target_y);
        int8_t   sign_x     = (init_x > target_x) ? -1 : 1;
        int8_t   sign_y     = (init_y > target_y) ? -1 : 1;
        uint16_t x          = init_x;
        uint16_t y          = init_y;

        if (sqrt(pow(distance_x, 2) + pow(distance_y, 2)) > seeing_distance)
        {
            return false;
        }

        if (distance_x > distance_y)
        {
            int32_t t = distance_y * 2 - distance_x;

            do
            {
                if ((map[x][y].type & TRAVERSIBLE) == false)
                    return false;

                if (t >= 0)
                {
                    y += sign_y;
                    t -= distance_x * 2;
                }
                x += sign_x;
                t += distance_y * 2;
            } while (x != target_x || y != target_y);
        }
        else
        {
            int32_t t = distance_x * 2 - distance_y;

            do
            {
                if ((map[x][y].type & TRAVERSIBLE) == false)
                    return false;

                if (t >= 0)
                {
                    x += sign_x;
                    t -= distance_y * 2;
                }
                y += sign_y;
                t += distance_x * 2;
            } while (x != target_x || y != target_y);
        }

        return true;
    }

public:
    LOS_System(GameMap &gamemap) : map{gamemap} {}

    void addEntity(EntityPtr &entity) { lines_of_sight.emplace(entity); }

    void deleteEntity(EntityPtr &entity) { lines_of_sight.erase(entity); }

    void calculateLOS(EntityPtr &entity)
    {
        auto coord_ptr = entity->getComponent<Coordinates>();
        auto los_ptr   = entity->getComponent<LOSComponent>();

        los_ptr->has_LOS_to_player =
            lineOfSightAlg(coord_ptr->x,
                           coord_ptr->y,
                           /*placeholder, here goes playerx*/ 0,
                           /*placeholder, here goes playery*/ 0,
                           los_ptr->seeing_distance);
    }

    void calculateAllLinesOfSight()
    {
        for (auto los_entity : lines_of_sight)
        {
            auto coord_ptr = los_entity->getComponent<Coordinates>();
            auto los_ptr   = los_entity->getComponent<LOSComponent>();

            los_ptr->has_LOS_to_player =
                lineOfSightAlg(coord_ptr->x,
                               coord_ptr->y,
                               /*placeholder, here goes playerx*/ 0,
                               /*placeholder, here goes playery*/ 0,
                               los_ptr->seeing_distance);
        }
    }
};

#endif /*LOSSYSTEM_H*/