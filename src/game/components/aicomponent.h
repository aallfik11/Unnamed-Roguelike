#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../component.h"
#include "../action.h"
#include "../ai_enum.h"
#include <functional>
#include <cstdint>

/*
 * Probably one of the only few components that will break the rule of being nothing but "data buckets"
 * however, due to the fact that some entities might contain different AI systems with different goals, behaviors etc.
 * I haven't come up with a better way to do this other than to just store the "AI function" and make the system call it
 */

class AIComponent : public Component
{
    // TO DO: instead of a function, create an enum for different functions(or just whole ai systems like idk, Monster Greedy, Monster Cowardly etc)
    //(most monsters have the same ones) and store this instead,
    //  make action system simply look at which ones have which actions defined and add them to according AI systems
public:
    AIType ai_type;
    AIState ai_state;

    AIComponent(AIType ai_type = AIType::AI_MONSTER_DEFAULT,
                AIState ai_state = AIState::WANDER_AROUND)
    {
        this->ai_type = ai_type;
    }
};

#endif /*AI_COMPONENT_H*/