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
    //TO DO: instead of a function, create an enum for different functions(or just whole ai systems like idk, Monster Greedy, Monster Cowardly etc) 
    //(most monsters have the same ones) and store this instead,
    // make action system simply look at which ones have which actions defined and add them to according AI systems
    std::function<Action(uint32_t)> action;
};

#endif /*ACTIONCOMPONENT_H*/