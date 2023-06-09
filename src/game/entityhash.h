#ifndef ENTITYHASH_H
#define ENTITYHASH_H
#include "entity.h"
#include "observerptr.h"
namespace std
{
template <> struct hash<observer_ptr<Entity>>
{
    auto operator()(const observer_ptr<Entity> &entity_ptr) const -> size_t
    {
        return hash<uint32_t>()(entity_ptr->getId());
    }
};
} // namespace std
#endif /*ENTITYHASH_H*/
