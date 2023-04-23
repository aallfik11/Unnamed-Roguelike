#ifndef HUNGERCOMPONENT_H
#define HUNGERCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <memory>

class HungerComponent : public Component
{

    HungerComponent *cloneImpl() const override
    {
        return new HungerComponent(this->hunger, this->max_hunger);
    }

public:
    uint8_t hunger; // can be used both by as hunger and as food value
    uint8_t max_hunger;
    HungerComponent(uint8_t hunger = 100, uint8_t max_hunger = 100)
    {
        this->hunger     = hunger;
        this->max_hunger = max_hunger;
    }

    std::unique_ptr<HungerComponent> clone() const
    {
        return std::unique_ptr<HungerComponent>(this->cloneImpl());
    }
};

#endif /*HUNGERCOMPONENT_H*/