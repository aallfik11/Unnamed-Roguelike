#ifndef WEAPONSLOT_H
#define WEAPONSLOT_H
#include "../component.h"
#include "../entity.h"
#include <memory>

class WeaponSlot : public Component
{

    WeaponSlot *cloneImpl() const override
    {
        return new WeaponSlot(this->weapon_item);
    }

public:
    Entity *weapon_item;

    WeaponSlot(Entity *weapon_item = nullptr)
    {
        this->weapon_item = weapon_item;
    }

    std::unique_ptr<WeaponSlot> clone() const
    {
        return std::unique_ptr<WeaponSlot>(this->cloneImpl());
    }
};

#endif /*WEAPONSLOT_H*/