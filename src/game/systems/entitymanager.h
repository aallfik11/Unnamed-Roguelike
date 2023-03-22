#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "../entity.h"
#include <unordered_map>
#include <cstdint>

class EntityManager
{
    // bool is purge flag,
    using EntityHashmap = std::unordered_map<uint32_t, std::pair<std::shared_ptr<Entity>, bool>>;
    EntityHashmap entities_;

public:
    uint32_t createEntity()
    {
        Entity *temp = new Entity();
        entities_.emplace(temp->get_id(), std::make_pair<std::shared_ptr<Entity>, bool>(std::shared_ptr<Entity>(temp), false));
        return temp->get_id();
    }

    std::shared_ptr<Entity> getEntity(uint32_t entity_id)
    {
        return entities_.at(entity_id).first;
    }

    EntityHashmap &getAllEntities()
    {
        return entities_;
    }

    void markForDeletion(uint32_t id)
    {
        entities_.at(id).second = true;
    }

    void purgeAllMarked()
    {
        for (auto it = entities_.begin(); it != entities_.end(); it++)
        {
            if (it->second.second == true)
                entities_.erase(it);
        }
    }
};

#endif /*ENTITYMANAGER*/
