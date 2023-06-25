#ifndef NAVMAPMANAGER
#define NAVMAPMANAGER
#include "../components/coordinates.h"
#include "../components/navmapcomponent.h"
#include "../entity.h"
#include "../globals.h"
#include "../system.h"
#include "../tile.h"
#include <cmath>
#include <istream>
#include <list>
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

    void resetNavMap(NavMap &nav_map);

    void bfsNavMap(uint16_t target_x,
                   uint16_t target_y,
                   uint16_t priority,
                   double   multiplier,
                   NavMap  &nav_map);

    void fillRandomTargets();

public:
    enum class Destination
    {
        AWAY_FROM,
        TOWARDS
    };

    NavMapManager();
    NavMapManager(observer_ptr<const GameMap>      map,
                  const observer_ptr<const Entity> player);

    void assignMap(observer_ptr<const GameMap> map);

    void assignPlayer(observer_ptr<const Entity> player);

    void initializeRandomTargets() noexcept(false);

    void calculatePlayerNavMap() noexcept(false);

    void switchToPlayerNavMap(NavMap &nav_map) const noexcept(false);

    void calculateNavMap(NavMap &nav_map, TargetTuple targets) noexcept(false);

    void assignRandomTarget(NavMap &nav_map) noexcept(false);

    bool compareNavtuple(
        const std::tuple<uint16_t, uint16_t, NavCell> &nt1,
        const std::tuple<uint16_t, uint16_t, NavCell> &nt2) const noexcept;

    std::tuple<uint16_t, uint16_t>
    nextBestCoordinates(observer_ptr<Entity> const entity,
                        Destination destination) const noexcept(false);

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;
};

#endif /*NAVMAPMANAGER*/
