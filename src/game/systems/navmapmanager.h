#ifndef NAVMAPMANAGER
#define NAVMAPMANAGER
#include "../components/coordinates.h"
#include "../components/navmapcomponent.h"
#include "../entity.h"
#include "../globals.h"
#include "../system.h"
#include "../tile.h"
#include <cmath>
#include <exception>
#include <functional>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <queue>
#include <random>
#include <unordered_map>
#include <vector>

class NavMapManager : public System
{

    using GameMap = std::vector<std::vector<Tile>>;
    using NavMap  = std::vector<std::vector<NavCell>>;
    using RandomTargets =
        std::list<std::pair</*random x*/ uint16_t, /*random y*/ uint16_t>>;
    using TargetTuple =
        std::vector<std::tuple</*target's x coordinate*/ uint16_t,
                               /*target's y coordinate*/ uint16_t,
                               /*target's initial score value*/ uint16_t,
                               /*target's score multiplier */ double>>;

    observer_ptr<const GameMap>             map_;
    RandomTargets                           random_targets_;
    std::random_device                      rd_;
    std::mt19937                            mt_engine_;
    std::uniform_int_distribution<uint16_t> distro_x_;
    std::uniform_int_distribution<uint16_t> distro_y_;
    NavMap                                  nav_to_player_;
    observer_ptr<const Coordinates>         player_coordinates_;

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

                if (!(adjx == 0 || adjx == (*map_).size() - 1 || adjy == 0 ||
                      adjy == (*map_)[0].size() - 1 ||
                      ((*map_)[adjx][adjy].type & (TileType::WALL)) !=
                          TileType::NONE ||
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
            while (((*map_)[random_target_x][random_target_y].type &
                    TileType::TRAVERSIBLE) == TileType::NONE)
            {
                random_target_x = distro_x_(mt_engine_);
                random_target_y = distro_y_(mt_engine_);
            }
            random_targets_.push_back({random_target_x, random_target_y});
        }
    }

public:
    enum class Destination
    {
        AWAY_FROM,
        TOWARDS
    };

    NavMapManager(observer_ptr<const GameMap>      map,
                  const observer_ptr<const Entity> player)
        : NavMapManager()
    {
        map_                = map;
        player_coordinates_ = player->getConstComponent<Coordinates>();
        // precomputing random targets for creatures to pick when wandering
        // around
        fillRandomTargets();
    }

    NavMapManager()
    {
        mt_engine_ = std::mt19937(rd_());
        distro_x_ = std::uniform_int_distribution<uint16_t>(1, G_MAP_WIDTH - 2);
        distro_y_ =
            std::uniform_int_distribution<uint16_t>(1, G_MAP_HEIGHT - 2);
        map_                = nullptr;
        player_coordinates_ = nullptr;
        NavCell initial;
        nav_to_player_ =
            NavMap(G_MAP_WIDTH, std::vector<NavCell>(G_MAP_HEIGHT, initial));
    }

    void assignMap(observer_ptr<const GameMap> map) { map_ = map; }
    void assignPlayer(observer_ptr<const Entity> player)
    {
        player_coordinates_ = player->getConstComponent<Coordinates>();
    }
    void initializeRandomTargets() noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error("NavMapManager: ERROR -> Map unassigned");
        fillRandomTargets();
    }

    void calculatePlayerNavMap() noexcept(false)
    {
        if (player_coordinates_ == nullptr)
            throw std::runtime_error(
                "NavMapManager: ERROR -> Player unassigned");
        resetNavMap(nav_to_player_);
        calculateNavMap(
            nav_to_player_,
            {
                {player_coordinates_->x, player_coordinates_->y, 0, 1.0}
        });
    }

    void switchToPlayerNavMap(NavMap &nav_map) const noexcept(false)
    {
        if (player_coordinates_ == nullptr)
            throw std::runtime_error(
                "NavMapManager: ERROR -> Player unassigned");
        nav_map = nav_to_player_;
    }

    void calculateNavMap(NavMap &nav_map, TargetTuple targets) noexcept(false)
    {

        if (map_ == nullptr)
            throw std::runtime_error("NavMapManager: ERROR -> Map unassigned");
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
                    if (((*map_)[x][y].type & (TileType::WALL)) !=
                        TileType::NONE)
                    {
                        main_navmap[x][y].score = (~0);
                        continue;
                    }

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

    void assignRandomTarget(NavMap &nav_map) noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error("NavMapManager: ERROR -> Map unassigned");
        auto random_target = random_targets_.front();
        random_targets_.pop_front();
        random_targets_.push_back(random_target);

        calculateNavMap(
            nav_map,
            {
                {random_target.first, random_target.second, 0, 1.0}
        });
    }

    bool compareNavtuple(
        const std::tuple<uint16_t, uint16_t, NavCell> &nt1,
        const std::tuple<uint16_t, uint16_t, NavCell> &nt2) const noexcept
    {
        if (std::get<2>(nt1).visited == false)
            return false;
        if (std::get<2>(nt2).visited == false)
            return true;

        auto x1 = std::get<0>(nt1);
        auto y1 = std::get<1>(nt1);

        auto x2 = std::get<0>(nt2);
        auto y2 = std::get<1>(nt2);

        if (((*map_)[x1][y1].type & TileType::HAS_CREATURE) != TileType::NONE)
        {
            return false;
        }

        if (((*map_)[x2][y2].type & TileType::HAS_CREATURE) != TileType::NONE)
        {
            return true;
        }

        return (std::get<2>(nt1).score < std::get<2>(nt2).score);
    }

    std::tuple<uint16_t, uint16_t>
    nextBestCoordinates(observer_ptr<Entity> const entity,
                        Destination destination) const noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error("NavMapManager: ERROR -> Map unassigned");
        if (entity == nullptr)
            throw std::runtime_error(
                "NavMapManager: ERROR -> entity is nullptr");

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

        auto compare = [this](const NavTuple &nt1, const NavTuple &nt2)
        { return compareNavtuple(nt1, nt2); };

        if (destination == Destination::AWAY_FROM)
        {
            result = std::max({up, down, left, right}, compare);
            auto x = std::get<0>(result);
            auto y = std::get<1>(result);

            if (((*map_)[x][y].type &
                 (TileType::HAS_CREATURE | TileType::WALL)) != TileType::NONE)
            {
                return {current_x, current_y};
            }

            if (nav_map[current_x][current_y].score >
                nav_map[std::get<0>(result)][std::get<1>(result)].score)
                return std::make_tuple(current_x, current_y);

            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
        else
        {
            if (nav_map[current_x][current_y].score == 0)
                return std::make_tuple(current_x, current_y);

            result = std::min({up, down, left, right}, compare);

            auto x = std::get<0>(result);
            auto y = std::get<1>(result);

            if (((*map_)[x][y].type &
                 (TileType::HAS_CREATURE | TileType::WALL)) != TileType::NONE)
            {
                return {current_x, current_y};
            }

            if (nav_map[current_x][current_y].score <
                nav_map[std::get<0>(result)][std::get<1>(result)].score)
                return std::make_tuple(current_x, current_y);

            return (std::make_tuple(get<0>(result), get<1>(result)));
        }
    }

    void updateData() override { calculatePlayerNavMap(); }
    void readSystemMessages() override {}
    void clearSystemMessages() override {
        (*System::system_messages_)[SystemType::NAVMAP_MANAGER].clear();
    }
    void resetSystem() override
    {
        clearSystemMessages();
        nav_to_player_.clear();
        map_ = nullptr;
        random_targets_.clear();
        player_coordinates_ = nullptr;
    }
};

#endif /*NAVMAPMANAGER*/
