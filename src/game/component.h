#ifndef COMPONENT_H
#define COMPONENT_H
#include <memory>

class Component
{
    virtual Component *cloneImpl() const = 0;

public:
    virtual ~Component() {}
    std::unique_ptr<Component> clone() const
    {
        return std::unique_ptr<Component>(this->cloneImpl());
    }
};

#endif /*COMPONENT_H*/