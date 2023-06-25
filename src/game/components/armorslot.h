#ifndef ARMORSLOT_H
#define ARMORSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <any>
#include <istream>
#include <list>
#include <memory>
#include <ostream>

class ArmorSlot : public Component, public EntityHolder
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    ArmorSlot *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    observer_ptr<Entity> armor_item;

    ArmorSlot(observer_ptr<Entity> armor_item = nullptr);
    ArmorSlot(const ArmorSlot &armor_slot);

    ComponentType getType() const override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*ARMORSLOT_H*/
