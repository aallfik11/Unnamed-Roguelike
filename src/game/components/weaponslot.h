#ifndef WEAPONSLOT_H
#define WEAPONSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include <istream>
#include <ostream>

class WeaponSlot : public Component, public EntityHolder
{

    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    WeaponSlot *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    observer_ptr<Entity> weapon_item;

    WeaponSlot(observer_ptr<Entity> weapon_item = nullptr);
    WeaponSlot(const WeaponSlot &weapon_slot);

    ComponentType getType() const override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*WEAPONSLOT_H*/
