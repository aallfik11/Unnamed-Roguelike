#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H
#include "../action.h"
#include "../ai_enum.h"
#include "../components/aicomponent.h"
#include "../entity.h"
#include "../globals.h"
#include "healthsystem.h"
#include "navmapmanager.h"
#include "positionsystem.h"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>

class AISystem
{

  using EntityId = uint32_t;
  using EntityPtr = std::shared_ptr<Entity>;
  // using AIMap = std::unordered_map<std::shared_ptr<Entity>, std::weak_ptr<AIComponent>>;
  using AISet = std::unordered_set<EntityPtr>;
  using StateArray = std::array<std::unordered_map<AIState, std::function<Action(EntityPtr, EntityPtr)>>, 7>;
  using GameMap = std::vector<std::vector<Tile>>;

  PositionSystem &positon_system_;
  HealthSystem &health_system_;
  NavMapManager &navigation_manager_;

  AISet ais_;
  StateArray states_;

  Action approachTarget(EntityPtr &caller, EntityPtr &target)
  {
    /*if player within range, attack
      if lost LOS of player, go to the last known coords
      if no player in LOS around last known coords, wander around
    */

    auto [x, y] = navigation_manager_.nextBestCoordinates(caller, NavMapManager::Destination::TOWARDS);
    positon_system_.updatePosition(caller, x, y);
  }

  Action runAway(EntityPtr &caller, EntityPtr &target)
  {
    // if there's a line of sight to the player, run away, else rest
  }
  Action rest(EntityPtr &caller, EntityPtr &target)
  {
    /*if hp high enough and there's a LOS to the player, approach
      if hp low and LOS to player then run away
      if hp high enough and no LOS to the player, wander around
    */
  }
  Action attack(EntityPtr &caller, EntityPtr &target)
  {
    /*if hp goes low, run away
      if player runs away, chase,
     */
  }
  Action interactWithObject(EntityPtr &caller, EntityPtr &target)
  {
    /*unused for now
     */
  }
  Action wanderAround(EntityPtr &caller, EntityPtr &target)
  {
    /*if player enters LOS, approach/run away (depending on the circumstances)
     */
  }
  Action special(EntityPtr &caller, EntityPtr &target)
  {
    /*unused for now
     */
  }

public:
  AISystem(GameMap &map,
           PositionSystem &position_system,
           HealthSystem &health_system,
           NavMapManager &nav_manager,
           bool &player_moved)
      : positon_system_{position_system},
        health_system_{health_system},
        navigation_manager_{nav_manager}
  {

    /*APPROACH TARGET*/
    states_[APPROACH_TARGET].emplace(ATTACK, attack); // if player close enough
    states_[APPROACH_TARGET].emplace(INTERACT_WITH_OBJECT, interactWithObject);
    states_[APPROACH_TARGET].emplace(WANDER_AROUND, wanderAround);
    /*APPROACH TARGET*/

    /*RUN_AWAY*/
    states_[RUN_AWAY].emplace(REST, rest); // if far enough from the player
    /*RUN_AWAY*/

    /*REST*/
    states_[REST].emplace(APPROACH_TARGET, approachTarget); // if healthy enough or target has high priority
    states_[REST].emplace(RUN_AWAY, runAway);               // if not healthy enough to fight and/or afraid/cowardly
    states_[REST].emplace(WANDER_AROUND, wanderAround);     // if healthy and nothing else to do
    /*REST*/

    /*ATTACK*/
    states_[ATTACK].emplace(RUN_AWAY, runAway);               // if not healthy enough
    states_[ATTACK].emplace(APPROACH_TARGET, approachTarget); // if target runs away and healthy enough
    states_[ATTACK].emplace(SPECIAL, special);                // if special conditions apply
    /*ATTACK*/

    /*INTERACT*/
    states_[INTERACT_WITH_OBJECT].emplace(WANDER_AROUND, wanderAround);     // if done interacting and nothing else to do
    states_[INTERACT_WITH_OBJECT].emplace(APPROACH_TARGET, approachTarget); // if a valid target in range
    states_[INTERACT_WITH_OBJECT].emplace(ATTACK, attack);                  // if a valid target directly near
    states_[INTERACT_WITH_OBJECT].emplace(RUN_AWAY, runAway);               // if not healthy enough to fight and/or afraid/cowardly
    /*INTERACT*/

    /*WANDER AROUND*/
    states_[WANDER_AROUND].emplace(INTERACT_WITH_OBJECT, interactWithObject); // if stumbled upon something interactable (might not use that)
    states_[WANDER_AROUND].emplace(APPROACH_TARGET, approachTarget);          // if a valid target in range
    states_[WANDER_AROUND].emplace(RUN_AWAY, runAway);                        // if not healthy enough to fight and/or afraid/cowardly
    /*WANDER AROUND*/

    /*SPECIAL*/
    states_[SPECIAL].emplace(APPROACH_TARGET, approachTarget); // if target not withing special range
    states_[SPECIAL].emplace(RUN_AWAY, runAway);               // depending on the special action
    states_[SPECIAL].emplace(ATTACK, attack);                  // depending on the special action
    states_[SPECIAL].emplace(REST, rest);                      // depending on the special action
    /*SPECIAL*/
  }

  void addEntityAI(EntityPtr &entity)
  {
    ais_.emplace(entity);
  }

  void deleteEntityAI(EntityPtr &entity)
  {
    ais_.erase(entity);
  }

  Action runAI(EntityPtr &entity)
  {
    if (!ais_.contains(entity))
      return Action::ERROR;
  }
};

#endif /*AI_SYSTEM_H*/