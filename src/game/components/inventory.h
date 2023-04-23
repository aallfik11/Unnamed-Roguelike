#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <list>
#include <memory>

class Entity;

class Inventory : public Component
{

    Inventory *cloneImpl() const override { return new Inventory(*this); }

public:
    std::list<Entity *> inventory;

    Inventory(){};

    Inventory(const Inventory &inventory_component)
    {
        this->inventory = inventory_component.inventory;
    }

    std::unique_ptr<Inventory> clone() const
    {
        return std::unique_ptr<Inventory>(this->cloneImpl());
    }
};
#endif /*INVENTORY_H*/