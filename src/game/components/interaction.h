#ifndef INTERACTION_H
#define INTERACTION_H
#include "../component.h"
#include <functional>

template <class T, class... Args>
class Interaction : public Component
{
    std::function<T(Args)> _interact;

public:
    Interaction(std::function<void(Args)> fn = 0)
        : _interact{fn}
    {
    }

    std::function<T(Args)> getInteraction()
    {
        return _interact;
    }

    setInteraction(std::function<T(Args)> fn)
    {
        _interact = fn;
    }

    T interact(Args... args)
    {
        return _interact(args);
    }
};
#endif /*INTERACTION_H*/