#ifndef HUNGERCOMPONENT_H
#define HUNGERCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class HungerComponent : public Component
{

    HungerComponent *cloneImpl() const override
    {
        return new HungerComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::HUNGER << ' ' << this->hunger << ' '
           << this->max_hunger << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->hunger >> this->max_hunger;
        return is;
    }

public:
    uint8_t hunger; // can be used both by as hunger and as food value
    uint8_t max_hunger;
    HungerComponent(uint8_t hunger = 100, uint8_t max_hunger = 100)
    {
        this->hunger     = hunger;
        this->max_hunger = max_hunger;
    }

    HungerComponent(const HungerComponent &hunger_component)
    {
        this->hunger     = hunger_component.hunger;
        this->max_hunger = hunger_component.max_hunger;
    }
};

#endif /*HUNGERCOMPONENT_H*/
