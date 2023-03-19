#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <list>

class Inventory : public Component
{
public:
    std::list<std::unique_ptr<Entity>> _inventory;
    void update(Entity *entity) {}
    void get() {}
    void set() {}
};
#endif /*INVENTORY_H*/