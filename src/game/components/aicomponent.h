#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../action.h"
#include "../ai_enum.h"
#include "../component.h"
#include <cstdint>
#include <functional>
#include <memory>

/*
 * Probably one of the only few components that will break the rule of being
 * nothing but "data buckets" however, due to the fact that some entities might
 * contain different AI systems with different goals, behaviors etc. I haven't
 * come up with a better way to do this other than to just store the
 * "AI function" and make the system call it
 */

class AIComponent : public Component
{
    // TO DO: instead of a function, create an enum for different functions(or
    // just whole ai systems like idk, Monster Greedy, Monster Cowardly etc)
    //(most monsters have the same ones) and store this instead,
    //  make action system simply look at which ones have which actions defined
    //  and add them to according AI systems

    AIComponent(const AIComponent &ai_component)
    {
        this->ai_type       = ai_component.ai_type;
        this->ai_state      = ai_component.ai_state;
        this->last_target_x = ai_component.last_target_x;
        this->last_target_y = ai_component.last_target_y;
    }

public:
    AIType   ai_type;
    AIState  ai_state;
    uint16_t last_target_x;
    uint16_t last_target_y;

    AIComponent(AIType  ai_type  = AIType::AI_MONSTER_DEFAULT,
                AIState ai_state = AIState::WANDER_AROUND)
    {
        this->ai_type  = ai_type;
        this->ai_state = ai_state;
        last_target_x  = 0;
        last_target_y  = 0;
    }

    AIComponent *clone() const override { return new AIComponent(*this); }
    // std::unique_ptr<AIComponent> clone() const
    // {
    //     return std::unique_ptr<AIComponent>(this->cloneImpl());
    // }
};

#endif /*AI_COMPONENT_H*/