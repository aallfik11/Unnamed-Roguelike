#ifndef ENTITYHOLDER_H
#define ENTITYHOLDER_H
// #include "entity.h"
#include <list>
#include <memory>
#include "observerptr.h"

class Entity;

class EntityHolder
{
public:
    virtual void
    loadEntities(std::list<observer_ptr<Entity>> entities) = 0;
};
#endif /*ENTITYHOLDER*/
