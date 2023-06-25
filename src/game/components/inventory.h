#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include <istream>
#include <list>
#include <ostream>

class Inventory : public Component, public EntityHolder
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    Inventory *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    std::list<observer_ptr<Entity>> inventory;

    Inventory() = default;
    Inventory(const Inventory &inventory_component);

    ComponentType getType() const override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};
#endif /*INVENTORY_H*/
