#ifndef HEALTH_H
#define HEALTH_H
#include "../component.h"
#include <cstdint>

class Health : public Component
{
public:
    uint16_t max_health_points;
    uint16_t current_health_points;
    Health(uint16_t max_hp = 0, uint16_t current_hp = 0)
    {
        max_health_points = max_hp;
        (current_hp > max_hp) ? current_health_points = max_hp
                              : current_health_points = current_hp;
    }
};

#endif /*HEALTH_H*/