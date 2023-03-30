#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <list>
#include <memory>

class Entity;

class Inventory : public Component
{
public:
    std::list<std::shared_ptr<Entity>> inventory_;
};
#endif /*INVENTORY_H*/