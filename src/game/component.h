#ifndef COMPONENT_H
#define COMPONENT_H
#include <memory>

class Component
{

public:
    virtual Component *clone()
        const = 0; // make sure to always wrap this in a proper smart pointer
    virtual ~Component() {}
    // virtual std::unique_ptr<Component> clone() const
    // {
    //     return std::unique_ptr<Component>(this->cloneImpl());
    // }
};

template <class Derived>
std::unique_ptr<Derived> castToComponent(std::unique_ptr<Component> &base_ptr)
{
    auto raw_base    = base_ptr.release();
    auto raw_derived = dynamic_cast<Derived *>(raw_base);
    return std::unique_ptr<Derived>(raw_derived);
}
#endif /*COMPONENT_H*/
