#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H
#include "../entity.h"
#include "../components/health.h"
#include <cstdint>
#include <unordered_map>
#include <cmath>

class HealthSystem
{
public:
    std::unordered_map<uint32_t, std::shared_ptr<Health>> healths;

    void add_or_deduce_health(uint32_t entity_id, uint32_t value)
    {
        // value can be negative
        auto health = healths.at(entity_id).get();
        if (value > 0)
            (health->max_health_points < health->current_health_points + value) ? health->current_health_points = health->max_health_points
                                                                                : health->current_health_points += value;
        else
            (health->current_health_points < abs(value)) ? health->current_health_points = 0
                                                         : health->current_health_points -= value;
    }

    uint32_t get_current_health(uint32_t entity_id)
    {
        return healths.at(entity_id).get()->current_health_points;
    }
    uint32_t get_max_health(uint32_t entity_id)
    {
        return healths.at(entity_id).get()->max_health_points;
    }
};

#endif /*HEALTHSYSTEM_H*/