#ifndef COMPONENT_H
#define COMPONENT_H
#include "itemtypes.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class Component
{
protected:
    virtual Component    *cloneImpl() const                 = 0;
    virtual std::ostream &serialize(std::ostream &os) const = 0;
    virtual std::istream &deserialize(std::istream &is)     = 0;

public:
    std::unique_ptr<Component> clone() const
    {
        return std::unique_ptr<Component>(this->cloneImpl());
    }

    virtual ~Component() {}

    // virtual std::unique_ptr<Component> deserialize(std::istream &os) = 0;

    friend std::ostream &operator<<(std::ostream &os, const Component *const c)
    {
        return c->serialize(os);
    };

    friend std::istream &operator>>(std::istream &is, Component *const c)
    {
        return c->deserialize(is);
    }
};

template <class Derived, class Base = Component>
std::unique_ptr<Derived> castToComponent(std::unique_ptr<Base> &&base_ptr)
{
    auto raw_base    = base_ptr.release();
    auto raw_derived = dynamic_cast<Derived *>(raw_base);
    return std::unique_ptr<Derived>(raw_derived);
}

enum class ComponentType : uint8_t
{
    AI,
    AMULETSLOT,
    ARMOR,
    ARMORSLOT,
    BUFF,
    COORDINATES,
    CRIT,
    DESCRIPTION,
    EFFECT,
    EXPERIENCE,
    HEALTH,
    HUNGER,
    INVENTORY,
    ITEM,
    LINEOFSIGHT,
    NAME,
    NAVMAP,
    TILE,
    WEAPON,
    WEAPONSLOT,

    _SIZE //DO NOT TOUCH
};

inline std::ostream &operator<<(std::ostream &os, const ComponentType &type)
{
    os << +static_cast<uint8_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, ComponentType &type)
{
    uint8_t temp;
    is >> temp;
    type = static_cast<ComponentType>(temp);
    return is;
}

#endif /*COMPONENT_H*/
