#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include <memory>
#include <list>

class Entity;

class Inventory : public Component
{
public:
    std::list<std::shared_ptr<Entity>> inventory_;
};
#endif /*INVENTORY_H*/