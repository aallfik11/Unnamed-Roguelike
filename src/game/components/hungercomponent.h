#ifndef HUNGERCOMPONENT_H
#define HUNGERCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class HungerComponent : public Component
{

    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto h = static_cast<const HungerComponent *>(c);
        return (this->hunger == h->hunger && this->max_hunger == h->max_hunger);
    }
    HungerComponent *cloneImpl() const override
    {
        return new HungerComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::HUNGER << ' ' << +this->hunger << ' '
           << +this->max_hunger << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        int hunger{},
            max_hunger{}; // since uint8_t is basically a char, reading is
                          // problematic, so I need temporary values

        is >> hunger >> max_hunger;
        this->hunger     = static_cast<uint8_t>(hunger);
        this->max_hunger = static_cast<uint8_t>(max_hunger);
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
