#ifndef ENTITYHOLDER_H
#define ENTITYHOLDER_H
// #include "entity.h"
#include <list>
#include <memory>

class Entity;

class EntityHolder
{
public:
    virtual void
    loadEntities(std::shared_ptr<std::list<Entity *>> &entities) = 0;
};
#endif /*ENTITYHOLDER*/
