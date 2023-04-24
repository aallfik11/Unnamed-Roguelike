#ifndef HEALTH_H
#define HEALTH_H
#include "../component.h"
#include <cstdint>
#include <memory>

class Health : public Component
{
    Health *cloneImpl() const override { return new Health(*this); }

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

    Health(const Health &hp)
    {
        this->max_health_points     = hp.max_health_points;
        this->current_health_points = hp.current_health_points;
        this->alive                 = hp.alive;
    }

    // std::unique_ptr<Health> clone() const
    // {
    //     return std::unique_ptr<Health>(this->cloneImpl());
    // }
};

#endif /*HEALTH_H*/
