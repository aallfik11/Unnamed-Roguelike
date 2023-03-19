#ifndef HEALTHREGISTRY_H
#define HEALTHREGISTRY_H
#include "../components/health.h"
#include "../entity.h"
#include <cstdint>
#include <unordered_map>
#include <cmath>

class HealthSystem
{
    std::unordered_map<uint_fast64_t, std::shared_ptr<Health>> healths;
    // std::unordered_map<uint_fast64_t, Health> healths;

public:
    void add_entity(const Entity* entity)
    {
        healths.insert(entity->get_id(), entity->get_component<Health>());
    }

    void remove_health_component(uint_fast64_t entity_id)
    {
        healths.erase(entity_id);
    }

    void purge(std::vector<uint_fast64_t> entity_ids)
    {
        for (auto entity_id : entity_ids)
        {
            healths.erase(entity_id);
        }
    }

    void add_or_deduce_health(uint_fast64_t entity_id, uint_fast64_t value)
    {
        // // value can be negative
        auto health = healths.at(entity_id).get();
        if (value > 0)
            (health->max_health_points < health->current_health_points + value) ? health->current_health_points = health->max_health_points
                                                                                : health->current_health_points += value;
        else
            (health->current_health_points < abs(value)) ? health->current_health_points = 0
                                                         : health->current_health_points -= value;
        // auto hp = healths.at(entity_id);
        // if (value > 0)
        // {
        //     (hp.max_health_points < hp.current_health_points + value) ? hp.current_health_points = hp.max_health_points
        //                                                               : hp.current_health_points += value;
        // }
        // else
        // {
        //     (hp.current_health_points < abs(value)) ? hp.current_health_points = 0
        //                                             : hp.current_health_points -= value;
        }
    }

    uint_fast64_t get_current_health(uint_fast64_t entity_id)
    {
        return healths.at(entity_id).current_health_points;
    }
    uint_fast64_t get_max_health(uint_fast64_t entity_id)
    {
        return healths.at(entity_id).max_health_points;
    }
};

#endif /*HEALTHREGISTRY_H*/