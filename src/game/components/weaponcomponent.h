#ifndef WEAPONCOMPONENT_H
#define WEAPONCOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class WeaponComponent : public Component
{

    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    WeaponComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint16_t damage;

    WeaponComponent(uint16_t damage = 1);
    WeaponComponent(const WeaponComponent &weapon_component);

    ComponentType getType() const override;
};

#endif /*WEAPONCOMPONENT_H*/
