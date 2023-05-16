#ifndef ARMORCOMPONENT_H
#define ARMORCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <ostream>

class ArmorComponent : public Component
{
    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto a = static_cast<const ArmorComponent *>(c);
        return (this->armor_class == a->armor_class);
    }
    ArmorComponent *cloneImpl() const override
    {
        return new ArmorComponent(*this);
    }

    virtual std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::ARMOR << ' ' << +this->armor_class << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        int armor_class{};
        is >> armor_class;
        this->armor_class = static_cast<uint8_t>(armor_class);
        return is;
    }

public:
    uint8_t armor_class; // formula: (damage*10) / armor_class, chance of
                         // deflection/evasion: armor_class/100;
    ArmorComponent(uint8_t armor_class = 10)
    {
        this->armor_class = armor_class;
    }

    ArmorComponent(const ArmorComponent &armor_component)
    {
        this->armor_class = armor_component.armor_class;
    }
    ComponentType getType() const override { return ComponentType::ARMOR; }
};

#endif /*ARMORCOMPONENT_H*/
