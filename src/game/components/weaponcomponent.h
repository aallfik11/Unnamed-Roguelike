#ifndef WEAPONCOMPONENT_H
#define WEAPONCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <memory>

class WeaponComponent : public Component
{

    WeaponComponent *cloneImpl() const override
    {
        return new WeaponComponent(*this);
    }

public:
    uint16_t damage;
    WeaponComponent(uint16_t damage = 1) { this->damage = damage; }
    WeaponComponent(const WeaponComponent &weapon_component)
    {
        this->damage = weapon_component.damage;
    }

    // std::unique_ptr<WeaponComponent> clone() const
    // {
    //     return std::unique_ptr<WeaponComponent>(this->cloneImpl());
    // }
};

#endif /*WEAPONCOMPONENT_H*/