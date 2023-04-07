#ifndef WEAPONCOMPONENT_H
#define WEAPONCOMPONENT_H
#include "../component.h"
#include <cstdint>

class WeaponComponent : public Component
{
public:
    uint16_t damage;
    WeaponComponent(uint16_t damage) { this->damage = damage; }

    WeaponComponent *clone() { return new WeaponComponent(this->damage); }
};

#endif /*WEAPONCOMPONENT_H*/