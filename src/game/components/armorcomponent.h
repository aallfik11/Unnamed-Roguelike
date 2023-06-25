#ifndef ARMORCOMPONENT_H
#define ARMORCOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class ArmorComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    ArmorComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint8_t armor_class; // formula: (damage*10) / armor_class, chance of
                         // deflection/evasion: armor_class/100;
    ArmorComponent(uint8_t armor_class = 10);
    ArmorComponent(const ArmorComponent &armor_component);

    ComponentType getType() const override;
};

#endif /*ARMORCOMPONENT_H*/
