#ifndef NAVMAPMANAGER
#define NAVMAPMANAGER
#include "../tile.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <cmath>

class NavMapManager
{

    struct NavCell : std::pair<uint32_t, bool>
    {
        uint32_t &score = this->first;
        bool &visited = this->second;
    };

    using GameMap = std::vector<std::vector<Tile>>;
    using NavMap = std::vector<std::vector<NavCell>>;
    using EntityId = uint32_t;

    std::unordered_map<EntityId, NavMap> nav_maps_;
    GameMap &map_;

    void resetNavMap(EntityId entity_id)
    {
        if (!nav_maps_.contains(entity_id))
            return;
        for (auto column : nav_maps_.at(entity_id))
        {
            for (auto cell : column)
            {
                cell.first = ~0;
                cell.second = false;
            }
        }
    }

    void bfsNavMap(uint16_t target_x,
                   uint16_t target_y,
                   uint16_t priority,
                   double multiplier,
                   NavMap &nav_map)
    {
        uint16_t vector_x[] = {-1, 0, 1, 0};
        uint16_t vector_y[] = {0, 1, 0, -1};
        std::queue<std::pair<uint16_t, uint16_t>> queue;
        queue.push({target_x, target_y});
        nav_map[target_x][target_y].visited = true;
        nav_map[target_x][target_y].score = 0;
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
        if ((std::fabs(multiplier - 1.0)) <= std::numeric_limits<double>::epsilon())
        {
            for (auto &col : nav_map)
                for (auto &cell : col)
                    cell.score *= multiplier;
        }
    }

public:
    void calculateNavMap(EntityId entity_id,
                         std::vector<std::tuple<uint16_t,
                                                uint16_t,
                                                uint16_t,
                                                double>>
                             targets)
    {

        std::vector<NavMap> nav_maps(targets.size()); // for each target there's a navmap

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
        auto &main_navmap = nav_maps[0];

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

        nav_maps_[entity_id] = main_navmap;
    }
};

#endif /*NAVMAPMANAGER*/