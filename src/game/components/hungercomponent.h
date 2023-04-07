#ifndef HUNGERCOMPONENT_H
#define HUNGERCOMPONENT_H
#include "../component.h"
#include <cstdint>

class HungerComponent : public Component
{
public:
    uint8_t hunger; // can be used both by as hunger and as food value
    uint8_t max_hunger;
    HungerComponent(uint8_t hunger = 100, uint8_t max_hunger = 100)
    {
        this->hunger     = hunger;
        this->max_hunger = max_hunger;
    }

    HungerComponent *clone()
    {
        return new HungerComponent(this->hunger, this->max_hunger);
    }
};

#endif /*HUNGERCOMPONENT_H*/