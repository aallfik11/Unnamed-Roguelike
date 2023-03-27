#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H
#include "../action.h"
#include "../ai_enum.h"
#include "../components/aicomponent.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <array>
#include <functional>

class AISystem
{

  //might need to include pos and health system here;
  using EntityId = uint32_t;
  using AIMap = std::unordered_map<EntityId, std::weak_ptr<AIComponent>>;
  using StateArray = std::array<std::array<std::function<Action(EntityId)>,7>,7>;

  AIMap ais;
  StateArray states;

  Action approachTarget(EntityId entity_id){}
  Action runAway(EntityId entity_id){}
  Action rest(EntityId entity_id){}
  Action attack(EntityId target){}
  Action interactWithObject(EntityId target){}
  Action approachTarget(EntityId entity_id){}
  Action special(EntityId target){}

public:

  AISystem()
  {
    states[APPROACH_TARGET] = {};
    states[RUN_AWAY] = {};
    states[REST] = {};
    states[ATTACK] = {};
    states[INTERACT_WITH_OBJECT] = {};
    states[WANDER_AROUND] = {};
    states[SPECIAL] = {};
  }

  void addEntityAI(EntityId entity_id, std::shared_ptr<AIComponent> &ai_component_ptr)
  {
    ais.emplace(entity_id, ai_component_ptr);
  }

  void deleteEntityAI(EntityId entity_id)
  {
    if (ais.contains(entity_id))
    {
      ais.erase(entity_id);
    }
  }

  Action runAI(EntityId entity_id)
  {
    if(!ais.contains(entity_id))
      return Action::ERROR;

    

  }
};

#endif /*AI_SYSTEM_H*/