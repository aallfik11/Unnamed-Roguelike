#ifndef WEAPONSLOT_H
#define WEAPONSLOT_H
#include "../component.h"
#include "../entity.h"
#include <memory>

class WeaponSlot : public Component
{
public:
    std::shared_ptr<Entity> weapon_item;

    WeaponSlot(std::shared_ptr<Entity> weapon_item)
    {
        this->weapon_item = weapon_item;
    }

    WeaponSlot *clone() { return new WeaponSlot(this->weapon_item); }
};

#endif /*WEAPONSLOT_H*/