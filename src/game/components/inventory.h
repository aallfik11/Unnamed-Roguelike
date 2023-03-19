#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <list>

class Inventory : public Component
{
public:
    std::list<std::shared_ptr<Entity>> _inventory;
};
#endif /*INVENTORY_H*/