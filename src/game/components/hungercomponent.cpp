#include "hungercomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
HungerComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto h = static_cast<const HungerComponent *>(c);
    return (this->hunger == h->hunger && this->max_hunger == h->max_hunger);
}
HungerComponent *HungerComponent::cloneImpl() const
{
    return new HungerComponent(*this);
}

std::ostream &HungerComponent::serialize(std::ostream &os) const
{
    os << ComponentType::HUNGER << ' ' << +this->hunger << ' '
       << +this->max_hunger << ' ';
    return os;
}

std::istream &HungerComponent::deserialize(std::istream &is)
{
    int hunger{}, max_hunger{}; // since uint8_t is basically a char, reading is
                                // problematic, so I need temporary values

    is >> hunger >> max_hunger;
    this->hunger     = static_cast<uint8_t>(hunger);
    this->max_hunger = static_cast<uint8_t>(max_hunger);
    return is;
}

HungerComponent::HungerComponent(uint8_t hunger, uint8_t max_hunger)
{
    this->hunger     = hunger;
    this->max_hunger = max_hunger;
}

HungerComponent::HungerComponent(const HungerComponent &hunger_component)
{
    this->hunger     = hunger_component.hunger;
    this->max_hunger = hunger_component.max_hunger;
}
ComponentType HungerComponent::getType() const { return ComponentType::HUNGER; }
