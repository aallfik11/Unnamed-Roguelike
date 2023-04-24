#ifndef COMPONENT_H
#define COMPONENT_H
#include <memory>

class Component
{
    virtual Component *cloneImpl() const = 0;

public:
    std::unique_ptr<Component> clone() const
    {
        return std::unique_ptr<Component>(this->cloneImpl());
    }
    virtual ~Component() {}
};

template <class Derived>
std::unique_ptr<Derived> castToComponent(std::unique_ptr<Component> &base_ptr)
{
    auto raw_base    = base_ptr.release();
    auto raw_derived = dynamic_cast<Derived *>(raw_base);
    return std::unique_ptr<Derived>(raw_derived);
}
#endif /*COMPONENT_H*/
