#ifndef HEALTH_H
#define HEALTH_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class Health : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    Health *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint16_t max_health_points; // get increased based on level, formula is:
                                // base max_hp +  base_max_hp * level /2
    uint16_t current_health_points;
    bool     alive;

    Health();
    Health(uint16_t max_hp);
    Health(uint16_t max_hp, uint16_t current_hp, bool alive = true);
    Health(const Health &hp);

    ComponentType getType() const override;
};

#endif /*HEALTH_H*/
