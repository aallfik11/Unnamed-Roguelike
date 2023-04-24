#ifndef COMPONENTCASTER_H
#define COMPONENTCASTER_H
#include "component.h"
#include <memory>

template <class Derived>
std::unique_ptr<Derived> castToComponent(std::unique_ptr<Component> &base_ptr)
{
    auto raw_base    = base_ptr.release();
    auto raw_derived = dynamic_cast<Derived *>(raw_base);
    return std::unique_ptr<Derived>(raw_derived);
}

#endif /*COMPONENTCASTER_H*/
