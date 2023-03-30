#ifndef NAVMAPMANAGER
#define NAVMAPMANAGER
#include "../components/coordinates.h"
#include "../components/navmapcomponent.h"
#include "../entity.h"
#include "../globals.h"
#include "../tile.h"
#include <cmath>
#include <memory>
#include <queue>
#include <unordered_map>
#include <vector>

class NavMapManager
{

    using GameMap = std::vector<std::vector<Tile>>;
    using NavMap = std::vector<std::vector<NavCell>>;
    using EntityId = uint32_t;
    using EntityPtr = std::shared_ptr<Entity>;

    // std::unordered_map<EntityPtr, NavMap> nav_maps_;
    GameMap &map_;

    void resetNavMap(EntityPtr &entity)
    {
        if (auto nav_map_component = entity->getComponent<NavMapComponent>())
        {
            for (auto column : nav_map_component->nav_map)
            {
                for (auto cell : column)
                {
                    cell.score = ~0;
                    cell.visited = false;
                }
            }
        }
    }

    void bfsNavMap(uint16_t target_x,
                   uint16_t target_y,
                   uint16_t priority,
                   double multiplier,
                   NavMap &nav_map)
    {
        int16_t vector_x[] = {-1, 0, 1, 0};
        int16_t vector_y[] = {0, 1, 0, -1};
        std::queue<std::pair<uint16_t, uint16_t>> queue;
        queue.push({target_x, target_y});
        nav_map[target_x][target_y].visited = true;
        nav_map[target_x][target_y].score = priority;
        uint32_t score = priority;

        while (!queue.empty())
        {
            score++;
            std::pair<uint16_t, uint16_t> cell = queue.front();
            uint16_t x = cell.first;
            uint16_t y = cell.second;

            queue.pop();
            for (int i = 0; i < 4; i++)
            {
                uint16_t adjx = x + vector_x[i];
                uint16_t adjy = y + vector_y[i];
                if (!(adjx == 0 ||
                      adjx == map_.size() - 1 ||
                      adjy == 0 ||
                      adjy == map_[0].size() - 1 ||
                      map_[adjx][adjy].type == TileType::WALL ||
                      nav_map[adjx][adjy].visited == true))
                {
                    queue.push({adjx, adjy});
                    nav_map[adjx][adjy].score = score;
                    nav_map[adjx][adjy].visited = true;
                }
            }
        }
        if ((std::fabs(multiplier - 1.0)) >= std::numeric_limits<double>::epsilon())
        {
            for (auto &col : nav_map)
                for (auto &cell : col)
                {
                    if (cell.visited)
                        cell.score *= multiplier;
                }
        }
    }

public:
    enum class Destination
    {
        AWAY_FROM,
        TOWARDS
    };

    NavMapManager(GameMap &map) : map_(map)
    {
    }

    void calculateNavMap(EntityPtr &entity,
                         std::vector<std::tuple<uint16_t,
                                                uint16_t,
                                                uint16_t,
                                                double>>
                             targets)
    {

        NavMap &nav_map = entity->getComponent<NavMapComponent>()->nav_map;

        std::vector<NavMap> nav_maps(targets.size(), nav_map); // for each target there's a navmap

        for (auto i = 0; i < targets.size(); i++)
        {
            bfsNavMap(std::get<0>(targets[i]),
                      std::get<1>(targets[i]),
                      std::get<2>(targets[i]),
                      std::get<3>(targets[i]),
                      nav_maps[i]);
        }
        auto navmap_x_size = nav_maps[0].size();
        auto navmap_y_size = nav_maps[0][0].size();
        NavMap &main_navmap = nav_maps[0];

        // combining all targets together, monsters will act on their most pressing desire
        for (auto navmap_id = 1; navmap_id < nav_maps.size(); navmap_id++)
        {
            auto &navmap_secondary = nav_maps[navmap_id];
            for (auto x = 0; x < navmap_x_size; x++)
            {
                for (auto y = 0; y < navmap_y_size; y++)
                {
                    if (main_navmap[x][y].score == ~0)
                        continue;

                    main_navmap[x][y].score = std::min(main_navmap[x][y].score, navmap_secondary[x][y].score);
                }
            }
        }

        for (auto x = 0; x < G_MAP_WIDTH; x++)
        {
            nav_map[x] = main_navmap[x];
        }
    }

    std::tuple<uint16_t, uint16_t> nextBestCoordinates(EntityPtr &entity, Destination destination)
    {
        auto nav_map = entity->getComponent<NavMapComponent>()->nav_map;
        auto current_coordinates = entity->getComponent<Coordinates>();
        auto current_x = current_coordinates->x;
        auto current_y = current_coordinates->y;

        using NavTuple = std::tuple<uint16_t, uint16_t, NavCell>;

        NavTuple up = {current_x, current_y - 1, nav_map[current_x][current_y - 1]};
        NavTuple down = {current_x, current_y + 1, nav_map[current_x][current_y - 1]};
        NavTuple left = {current_x - 1, current_y, nav_map[current_x][current_y - 1]};
        NavTuple right = {current_x + 1, current_y, nav_map[current_x][current_y - 1]};

        auto compare_higher = [](const NavTuple &nt1, const NavTuple &nt2)
        {
            if (std::get<2>(nt1).visited == false)
                return false;

            return (std::get<2>(nt1).score > std::get<2>(nt2).score);
        };
        auto compare_lower = [](const NavTuple &nt1, const NavTuple &nt2)
        {
            if (std::get<2>(nt1).visited == false)
                return false;

            return (std::get<2>(nt1).score < std::get<2>(nt2).score);
        };

        if (destination == Destination::AWAY_FROM)
        {
            NavTuple result = std::max(std::max(up, down, compare_higher),
                                       std::max(left, right, compare_higher),
                                       compare_higher);
            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
        else
        {
            NavTuple result = std::min(std::min(up, down, compare_lower),
                                       std::min(left, right, compare_lower),
                                       compare_lower);
            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
    }
};

#endif /*NAVMAPMANAGER*/