#ifndef INTERACTION_H
#define INTERACTION_H
#include "../component.h"
#include <functional>

template <class T, class... Args>
class Interaction : Component
{
    std::function<T(Args)> _interact;

public:
    Interaction(std::function<void(Args)> fn = 0)
        : _interact{fn}
    {
    }

    std::function<T(Args)> get_interaction()
    {
        return _interact;
    }

    set_interaction(std::function<T(Args)> fn)
    {
        _interact = fn;
    }

    T interact(Args... args)
    {
        return _interact(args);
    }
};
#endif /*INTERACTION_H*/