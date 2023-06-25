#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H
#include "../ai_enum.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include "navmapmanager.h"
#include <cstdint>
#include <istream>
#include <list>
#include <ostream>
#include <unordered_set>

class AISystem : public System, public EntityHolder
{

    /* In order to work correctly, an entity in the AI System is required
     * to have the following components:
     * AIComponent
     * NavMapComponent
     * LOSComponent
     * Health
     * Coordinates
     * WeaponComponent
     * StrengthComponent
     */

    using AISet   = std::unordered_set<observer_ptr<Entity>>;
    using GameMap = std::vector<std::vector<Tile>>;

    AISet                           ais_;
    observer_ptr<const GameMap>     map_;
    observer_ptr<NavMapManager>     navigation_manager_;
    observer_ptr<Entity>            player_;
    std::list<observer_ptr<Entity>> addition_messages_;
    std::list<observer_ptr<Entity>> removal_messages_;

    double getRunThreshold(AIType &ai_type);

    void approachTarget(const observer_ptr<Entity> caller,
                        const observer_ptr<Entity> target);

    void runAway(const observer_ptr<Entity> caller,
                 const observer_ptr<Entity> target);

    void rest(const observer_ptr<Entity> caller,
              const observer_ptr<Entity> target);

    void attack(const observer_ptr<Entity> caller,
                const observer_ptr<Entity> target);

    void wanderAround(const observer_ptr<Entity> caller,
                      const observer_ptr<Entity> target);

public:
    AISystem();
    AISystem(const observer_ptr<const GameMap> map,
             observer_ptr<NavMapManager>       nav_manager,
             observer_ptr<Entity> const        player);

    void assignPlayer(const observer_ptr<Entity> player);

    void assignNavMapManager(const observer_ptr<NavMapManager> nav_manager);

    void assignMap(const observer_ptr<const GameMap> map);

    void addEntity(const observer_ptr<Entity> entity);

    void deleteEntity(const observer_ptr<Entity> entity);

    void runAI(const observer_ptr<Entity> caller,
               const observer_ptr<Entity> target);

    void updateData() noexcept(false) override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*AI_SYSTEM_H*/
