#ifndef ACTIONCOMPONENT_H
#define ACTIONCOMPONENT_H
#include "../component.h"
#include "../action.h"
#include <functional>
#include <cstdint>

/*
 * Probably one of the only few components that will break the rule of being nothing but "data buckets"
 * however, due to the fact that some entities might contain different AI systems with different goals, behaviors etc.
 * I haven't come up with a better way to do this other than to just store the "AI function" and make the system call it
 */

class ActionComponent : public Component
{
    std::function<Action(uint32_t)> action;
};

#endif /*ACTIONCOMPONENT_H*/