#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H
#include "../components/armorcomponent.h"
#include "../components/health.h"
#include "../components/weaponcomponent.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../health_enum.h"
#include <cstdint>
#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class HealthSystem
{
    using EntityId  = uint32_t;
    using HealthPtr = std::shared_ptr<Health>;
    using EntityPtr = std::shared_ptr<Entity>;

    // std::unordered_map<EntityId, std::weak_ptr<Health>> health_register_;
    std::unordered_set<EntityPtr> health_register_;
    std::random_device            rd;
    std::mt19937                  mt_engine;

public:
    HealthSystem() { mt_engine = std::mt19937(rd()); }

    HealthSystem(const std::initializer_list<EntityPtr> &entities)
        : HealthSystem()
    {
        for (auto &entity : entities)
        {
            health_register_.emplace(entity);
        }
    }

    void addEntity(EntityPtr &entity) { health_register_.emplace(entity); }

    void deleteEntity(EntityPtr &entity) { health_register_.erase(entity); }

    uint16_t getHealth(EntityPtr &entity, HealthAction action)
    {
        if (health_register_.contains(entity) == false)
            return 0;

        if (auto health_ptr = entity->getComponent<Health>())
        {
            uint16_t health = (action & CURRENT)
                                  ? health_ptr->current_health_points
                                  : health_ptr->max_health_points;
            return health;
        }
        deleteEntity(entity);
        return ~0;
    }

    void updateHealth(EntityPtr &entity, uint16_t amount, HealthAction action)
    {
        if (!health_register_.contains(entity))
            return;

        if (auto health_ptr = entity->getComponent<Health>())
        {
            uint16_t current_health = health_ptr->current_health_points;
            uint16_t max_health     = health_ptr->max_health_points;
            if (action & CURRENT)
            {

                if (action & DEDUCE)
                {
                    uint16_t damage = amount;
                    if (auto armor_ptr = entity->getComponent<ArmorComponent>())
                    {
                        std::uniform_int_distribution miss_chance(
                            1, 100 / armor_ptr->armor_class);
                        if (miss_chance(mt_engine) == 1)
                        {
                            // message for miss goes here
                            return;
                        }
                        damage = ((amount * 10) / armor_ptr->armor_class);
                    }
                    if (damage >= current_health)
                    {

                        health_ptr->current_health_points = 0;
                        health_ptr->alive                 = false;
                        entity->type &= ~EntityType::LIVING;
                        entity->type |= EntityType::KILLED;
                    }
                    else
                        health_ptr->current_health_points -= amount;
                }
                else
                {
                    health_ptr->current_health_points =
                        (amount + current_health >= max_health)
                            ? max_health
                            : amount + current_health;
                }
            }
            else
            {
                if (action & DEDUCE)
                {
                    if (amount >= max_health)
                    {
                        health_ptr->max_health_points     = 0;
                        health_ptr->current_health_points = 0;
                    }
                    else
                    {
                        health_ptr->max_health_points = max_health - amount;
                        if (current_health > health_ptr->max_health_points)
                            health_ptr->current_health_points =
                                health_ptr->max_health_points;
                    }
                }
                else
                    health_ptr->max_health_points = max_health + amount;
            }
        }
        else
            deleteEntity(entity);
    }
};

#endif /*HEALTHSYSTEM_H*/