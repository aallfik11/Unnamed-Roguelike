#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H
#include "../action.h"
#include "../ai_enum.h"
#include "../components/aicomponent.h"
#include "../entity.h"
#include "positionsystem.h"
#include "healthsystem.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <functional>

class AISystem
{

  using EntityId = uint32_t;
  using AIMap = std::unordered_map<std::shared_ptr<Entity>, std::weak_ptr<AIComponent>>;
  using AISet = std::unordered_set<std::shared_ptr<Entity>>;
  using StateArray = std::array<std::unordered_map<AIState, std::function<Action(EntityId, EntityId)>>, 7>;

  PositionSystem &positon_system_;
  HealthSystem &health_system_;

  AISet ais;
  StateArray states;

  Action approachTarget(EntityId caller_id, EntityId target_id)
  {
  }
  Action runAway(EntityId caller_id, EntityId target_id)
  {
  }
  Action rest(EntityId caller_id, EntityId)
  {
  }
  Action attack(EntityId caller_id, EntityId target_id)
  {
  }
  Action interactWithObject(EntityId caller_id, EntityId target_id)
  {
  }
  Action wanderAround(EntityId caller_id, EntityId)
  {
  }
  Action special(EntityId caller_id, EntityId target_id)
  {
  }

public:
  AISystem(PositionSystem &position_system, HealthSystem &health_system)
      : positon_system_{position_system},
        health_system_{health_system}
  {
    /*APPROACH TARGET*/
    states[APPROACH_TARGET].emplace(ATTACK, attack); // if player close enough
    states[APPROACH_TARGET].emplace(INTERACT_WITH_OBJECT, interactWithObject);
    states[APPROACH_TARGET].emplace(WANDER_AROUND, wanderAround);
    /*APPROACH TARGET*/

    /*RUN_AWAY*/
    states[RUN_AWAY].emplace(REST, rest); // if far enough from the player
    /*RUN_AWAY*/

    /*REST*/
    states[REST].emplace(APPROACH_TARGET, approachTarget); // if healthy enough or target has high priority
    states[REST].emplace(RUN_AWAY, runAway);               // if not healthy enough to fight and/or afraid/cowardly
    states[REST].emplace(WANDER_AROUND, wanderAround);     // if healthy and nothing else to do
    /*REST*/

    /*ATTACK*/
    states[ATTACK].emplace(RUN_AWAY, runAway);               // if not healthy enough
    states[ATTACK].emplace(APPROACH_TARGET, approachTarget); // if target runs away and healthy enough
    states[ATTACK].emplace(SPECIAL, special);                // if special conditions apply
    /*ATTACK*/

    /*INTERACT*/
    states[INTERACT_WITH_OBJECT].emplace(WANDER_AROUND, wanderAround);     // if done interacting and nothing else to do
    states[INTERACT_WITH_OBJECT].emplace(APPROACH_TARGET, approachTarget); // if a valid target in range
    states[INTERACT_WITH_OBJECT].emplace(ATTACK, attack);                  // if a valid target directly near
    states[INTERACT_WITH_OBJECT].emplace(RUN_AWAY, runAway);               // if not healthy enough to fight and/or afraid/cowardly
    /*INTERACT*/

    /*WANDER AROUND*/
    states[WANDER_AROUND].emplace(INTERACT_WITH_OBJECT, interactWithObject); // if stumbled upon something interactable (might not use that)
    states[WANDER_AROUND].emplace(APPROACH_TARGET, approachTarget);          // if a valid target in range
    states[WANDER_AROUND].emplace(RUN_AWAY, runAway);                        // if not healthy enough to fight and/or afraid/cowardly
    /*WANDER AROUND*/

    /*SPECIAL*/
    states[SPECIAL].emplace(APPROACH_TARGET, approachTarget); // if target not withing special range
    states[SPECIAL].emplace(RUN_AWAY, runAway);               // depending on the special action
    states[SPECIAL].emplace(ATTACK, attack);                  // depending on the special action
    states[SPECIAL].emplace(REST, rest);                      // depending on the special action
    /*SPECIAL*/
  }

  void addEntityAI(std::shared_ptr<Entity> &entity_ptr)
  {
    ais.emplace(entity_ptr);
  }

  void deleteEntityAI(std::shared_ptr<Entity> &entity_ptr)
  {
    if (ais.contains(entity_ptr))
    {
      ais.erase(entity_ptr);
    }
  }

  Action runAI(std::shared_ptr<Entity> &entity_ptr)
  {
    if (!ais.contains(entity_ptr))
      return Action::ERROR;
  }
};

#endif /*AI_SYSTEM_H*/