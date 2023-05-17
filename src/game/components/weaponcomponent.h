#ifndef WEAPONCOMPONENT_H
#define WEAPONCOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class WeaponComponent : public Component
{

    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto w = static_observer_cast<const WeaponComponent>(c);
        return (this->damage == w->damage);
    }
    WeaponComponent *cloneImpl() const override
    {
        return new WeaponComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::WEAPON << ' ' << this->damage << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->damage;
        return is;
    }

public:
    uint16_t damage;
    WeaponComponent(uint16_t damage = 1) { this->damage = damage; }
    WeaponComponent(const WeaponComponent &weapon_component)
    {
        this->damage = weapon_component.damage;
    }
    ComponentType getType() const override { return ComponentType::WEAPON; }
};

#endif /*WEAPONCOMPONENT_H*/
