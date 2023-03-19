#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "../entity.h"
#include <unordered_map>
#include <cstdint>

class EntityManager
{
    std::unordered_map<std::shared_ptr<Entity>, bool> entities; // bool is purge flag,

public:
    uint32_t create_entity()
    {
        entities.insert(std::pair<uint32_t, bool>(entity, false));
        return entity;
    }

    std::unordered_map<uint32_t, bool> &get_all_entities()
    {
        return entities;
    }

    void mark_for_deletion(uint32_t id)
    {
        entities.at(id) = true;
    }

    void purge_all_marked()
    {
        for (auto it = entities.begin(); it != entities.end(); it++)
        {
            if (it->second == true)
                entities.erase(it);
        }
    }
};

#endif /*ENTITYMANAGER*/
