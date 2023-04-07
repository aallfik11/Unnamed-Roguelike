#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <list>
#include <memory>

class Entity;

class Inventory : public Component
{
public:
    std::list<std::shared_ptr<Entity>> inventory;

    Inventory(){};

    Inventory(std::list<std::shared_ptr<Entity>> inventory)
    {
        this->inventory = inventory;
    }

    Inventory *clone() { return new Inventory(this->inventory); }
};
#endif /*INVENTORY_H*/