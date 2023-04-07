#ifndef HEALTH_H
#define HEALTH_H
#include "../component.h"
#include <cstdint>

class Health : public Component
{
public:
    uint16_t max_health_points; // get increased based on level, formula is:
                                // base max_hp +  base_max_hp * level /2
    uint16_t current_health_points;
    bool     alive;

    Health(uint16_t max_hp = 0, uint16_t current_hp = 0, bool alive = false)
    {
        max_health_points     = max_hp;

        current_health_points = (current_hp > max_hp) ? max_hp : current_hp;
        this->alive           = alive;
    }

    Health *clone()
    {
        return new Health(
            this->max_health_points, this->current_health_points, this->alive);
    }
};

#endif /*HEALTH_H*/