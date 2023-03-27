#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H
#include "../components/health.h"
#include "../entity.h"
#include "../health_enum.h"
#include <cstdint>
#include <unordered_map>
#include <memory>

class HealthSystem
{
    using EntityId = uint32_t;
    using HealthPtr = std::shared_ptr<Health>;

    std::unordered_map<EntityId, std::weak_ptr<Health>> health_register;

public:
    void addEntity(EntityId entity_id, std::shared_ptr<Health> &health_component)
    {
        health_register.emplace(entity_id, health_component);
    }

    void deleteEntity(EntityId entity_id)
    {
        if (health_register.contains(entity_id))
        {
            health_register.erase(entity_id);
        }
    }

    uint16_t getHealth(EntityId entity_id, HealthAction action)
    {
        if (!health_register.contains(entity_id))
            return 0;

        if (auto health_ptr = health_register.at(entity_id).lock())
        {
            uint16_t health = (action & CURRENT) ? health_ptr->current_health_points
                                                 : health_ptr->max_health_points;
            return health;
        }
        deleteEntity(entity_id);
        return 0;
    }

    void updateHealth(EntityId entity_id, uint16_t amount, HealthAction action)
    {
        if (!health_register.contains(entity_id))
            return;

        if (auto health_ptr = health_register.at(entity_id).lock())
        {
            uint16_t current_health = health_ptr->current_health_points;
            uint16_t max_health = health_ptr->max_health_points;
            if (action & CURRENT)
            {

                if (action & DEDUCE)
                {
                    if(amount >= current_health)
                    {
                        health_ptr->current_health_points = 0;
                        health_ptr->alive = false;
                    }
                    else
                        health_ptr->current_health_points -= amount;
                }
                else
                {
                    health_ptr->current_health_points = (amount + current_health >= max_health) ? max_health
                                                                                                : amount + current_health;
                }
            }
            else
            {
                if (action & DEDUCE)
                {
                    if (amount >= max_health)
                    {
                        health_ptr->max_health_points = 0;
                        health_ptr->current_health_points = 0;
                    }
                    else
                    {
                        health_ptr->max_health_points = max_health - amount;
                        if (current_health > health_ptr->max_health_points)
                            health_ptr->current_health_points = health_ptr->max_health_points;
                    }
                }
                else
                    health_ptr->max_health_points = max_health + amount;
            }
        }
        else
            deleteEntity(entity_id);
    }
};

#endif /*HEALTHSYSTEM_H*/