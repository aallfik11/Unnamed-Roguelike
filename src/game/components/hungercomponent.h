#ifndef HUNGERCOMPONENT_H
#define HUNGERCOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class HungerComponent : public Component
{

    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    HungerComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint8_t hunger; // can be used both by as hunger and as food value
    uint8_t max_hunger;

    HungerComponent(uint8_t hunger = 100, uint8_t max_hunger = 100);
    HungerComponent(const HungerComponent &hunger_component);

    ComponentType getType() const override;
};

#endif /*HUNGERCOMPONENT_H*/
