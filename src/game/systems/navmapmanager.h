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
#include <random>
#include <unordered_map>
#include <vector>

class NavMapManager
{

    using GameMap   = std::vector<std::vector<Tile>>;
    using NavMap    = std::vector<std::vector<NavCell>>;
    using EntityId  = uint32_t;
    using EntityPtr = std::shared_ptr<Entity>;
    using RandomTargets =
        std::queue<std::pair</*random x*/ uint16_t, /*random y*/ uint16_t>>;
    using TargetTuple =
        std::vector<std::tuple</*target's x coordinate*/ uint16_t,
                               /*target's y coordinate*/ uint16_t,
                               /*target's initial score value*/ uint16_t,
                               /*target's score multiplier */ double>>;

    GameMap                                &map_;
    RandomTargets                           random_targets_;
    std::random_device                      rd_;
    std::mt19937                            mt_engine_;
    std::uniform_int_distribution<uint16_t> distro_x_;
    std::uniform_int_distribution<uint16_t> distro_y_;
    NavMap                                  nav_to_player_;

    // std::unordered_map<EntityPtr, NavMap> nav_maps_;

    void resetNavMap(NavMap &nav_map)
    {
        for (auto &column : nav_map)
        {
            for (auto &cell : column)
            {
                cell.score   = ~0;
                cell.visited = false;
            }
        }
    }

    void bfsNavMap(uint16_t target_x,
                   uint16_t target_y,
                   uint16_t priority,
                   double   multiplier,
                   NavMap  &nav_map)
    {
        int16_t vector_x[] = {-1, 0, 1, 0};
        int16_t vector_y[] = {0, 1, 0, -1};

        std::queue<std::pair<uint16_t, uint16_t>> queue;
        queue.push({target_x, target_y});

        nav_map[target_x][target_y].visited = true;
        nav_map[target_x][target_y].score   = priority;
        uint32_t score                      = priority;

        while (!queue.empty())
        {
            score++;
            std::pair<uint16_t, uint16_t> cell = queue.front();
            uint16_t                      x    = cell.first;
            uint16_t                      y    = cell.second;

            queue.pop();
            for (int i = 0; i < 4; i++)
            {
                uint16_t adjx = x + vector_x[i];
                uint16_t adjy = y + vector_y[i];

                if (!(adjx == 0 || adjx == map_.size() - 1 || adjy == 0 ||
                      adjy == map_[0].size() - 1 ||
                      map_[adjx][adjy].type == TileType::WALL ||
                      nav_map[adjx][adjy].visited == true))
                {
                    queue.push({adjx, adjy});
                    nav_map[adjx][adjy].score   = score;
                    nav_map[adjx][adjy].visited = true;
                }
            }
        }

        if ((std::fabs(multiplier - 1.0)) >=
            std::numeric_limits<double>::epsilon())
        {
            for (auto &col : nav_map)
                for (auto &cell : col)
                {
                    if (cell.visited)
                        cell.score *= multiplier;
                }
        }
    }

    void fillRandomTargets()
    {
        while (random_targets_.size() <= 200)
        {
            auto random_target_x = distro_x_(mt_engine_);
            auto random_target_y = distro_y_(mt_engine_);
            while ((map_[random_target_x][random_target_y].type &
                    TRAVERSIBLE) == false)
            {
                random_target_x = distro_x_(mt_engine_);
                random_target_y = distro_y_(mt_engine_);
            }
            random_targets_.push({random_target_x, random_target_y});
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
        mt_engine_ = std::mt19937(rd_());
        distro_x_ = std::uniform_int_distribution<uint16_t>(1, G_MAP_WIDTH - 2);
        distro_y_ =
            std::uniform_int_distribution<uint16_t>(1, G_MAP_HEIGHT - 2);

        // precomputing random targets for creatures to pick when wandering
        // around
        while (random_targets_.size() <= 100)
        {
            auto random_target_x = distro_x_(mt_engine_);
            auto random_target_y = distro_y_(mt_engine_);
            while ((map_[random_target_x][random_target_y].type &
                    TRAVERSIBLE) == false)
            {
                random_target_x = distro_x_(mt_engine_);
                random_target_y = distro_y_(mt_engine_);
            }
            random_targets_.push({random_target_x, random_target_y});
        }

        NavCell initial;
        nav_to_player_ =
            NavMap(G_MAP_WIDTH, std::vector<NavCell>(G_MAP_HEIGHT, initial));
    }

    void calculatePlayerNavMap(
        const std::shared_ptr<Coordinates> &player_coordinates)
    {
        resetNavMap(nav_to_player_);
        calculateNavMap(
            nav_to_player_,
            {
                {player_coordinates->x, player_coordinates->y, 0, 1.0}
        });
    }

    void switchToPlayerNavMap(NavMap &nav_map) { nav_map = nav_to_player_; }

    void calculateNavMap(NavMap &nav_map, TargetTuple targets)
    {

        // NavMap &nav_map = entity->getComponent<NavMapComponent>()->nav_map;

        resetNavMap(nav_map);

        std::vector<NavMap> nav_maps(
            targets.size(), nav_map); // for each target there's a navmap

        for (size_t i = 0; i < targets.size(); i++)
        {
            bfsNavMap(std::get<0>(targets[i]),
                      std::get<1>(targets[i]),
                      std::get<2>(targets[i]),
                      std::get<3>(targets[i]),
                      nav_maps[i]);
        }
        auto    navmap_x_size = nav_maps[0].size();
        auto    navmap_y_size = nav_maps[0][0].size();
        NavMap &main_navmap   = nav_maps[0];

        // combining all targets together, monsters will act on their most
        // pressing desire
        for (size_t navmap_id = 1; navmap_id < nav_maps.size(); navmap_id++)
        {
            auto &navmap_secondary = nav_maps[navmap_id];
            for (size_t x = 0; x < navmap_x_size; x++)
            {
                for (size_t y = 0; y < navmap_y_size; y++)
                {
                    if (main_navmap[x][y].score == static_cast<uint32_t>(~0))
                        continue;

                    main_navmap[x][y].score = std::min(
                        main_navmap[x][y].score, navmap_secondary[x][y].score);
                }
            }
        }

        nav_map = std::move(main_navmap);
        // for (auto x = 0; x < G_MAP_WIDTH; x++)
        // {
        //     nav_map[x] = main_navmap[x];
        // }
    }

    void assignRandomTarget(NavMap &nav_map)
    {
        auto random_target = random_targets_.front();
        random_targets_.pop();
        random_targets_.push(random_target);

        calculateNavMap(
            nav_map,
            {
                {random_target.first, random_target.second, 0, 1.0}
        });
    }

    std::tuple<uint16_t, uint16_t> nextBestCoordinates(EntityPtr  &entity,
                                                       Destination destination)
    {
        using NavTuple = std::tuple<uint16_t, uint16_t, NavCell>;
        auto nav_map   = entity->getComponent<NavMapComponent>()->nav_map;

        auto current_coordinates = entity->getComponent<Coordinates>();
        auto current_x           = current_coordinates->x;
        auto current_y           = current_coordinates->y;

        NavTuple result;

        NavTuple up = {
            current_x, current_y - 1, nav_map[current_x][current_y - 1]};
        NavTuple down = {
            current_x, current_y + 1, nav_map[current_x][current_y + 1]};
        NavTuple left = {
            current_x - 1, current_y, nav_map[current_x - 1][current_y]};
        NavTuple right = {
            current_x + 1, current_y, nav_map[current_x + 1][current_y]};

        auto compare_higher = [](const NavTuple &nt1, const NavTuple &nt2)
        {
            if (std::get<2>(nt1).visited == false)
                return true;
            if (std::get<2>(nt2).visited == false)
                return false;

            return (std::get<2>(nt1).score < std::get<2>(nt2).score);
        };
        auto compare_lower = [](const NavTuple &nt1, const NavTuple &nt2)
        {
            if (std::get<2>(nt1).visited == false)
                return false;

            return (std::get<2>(nt1).score < std::get<2>(nt2).score);
        };

        if (destination == Destination::AWAY_FROM)
        {
            result = std::max(std::max(up, down, compare_higher),
                              std::max(left, right, compare_higher),
                              compare_higher);

            if (nav_map[current_x][current_y].score >
                nav_map[std::get<0>(result)][std::get<1>(result)].score)
                return std::make_tuple(current_x, current_y);

            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
        else
        {
            if (nav_map[current_x][current_y].score == 0)
                return std::make_tuple(current_x, current_y);

            result = std::min(std::min(up, down, compare_lower),
                              std::min(left, right, compare_lower),
                              compare_lower);

            if (nav_map[current_x][current_y].score <
                nav_map[std::get<0>(result)][std::get<1>(result)].score)
                return std::make_tuple(current_x, current_y);

            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
    }
};

#endif /*NAVMAPMANAGER*/