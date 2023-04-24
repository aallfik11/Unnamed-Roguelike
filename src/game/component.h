#ifndef COMPONENT_H
#define COMPONENT_H
#include <memory>

class Component
{

public:
    virtual Component *clone() const = 0;
    virtual ~Component() {}
    // virtual std::unique_ptr<Component> clone() const
    // {
    //     return std::unique_ptr<Component>(this->cloneImpl());
    // }
};

#endif /*COMPONENT_H*/