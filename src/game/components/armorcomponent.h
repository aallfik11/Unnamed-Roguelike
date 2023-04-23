#ifndef ARMORCOMPONENT_H
#define ARMORCOMPONENT_H
#include "../component.h"
#include <cstdint>

class ArmorComponent : public Component
{
    ArmorComponent *cloneImpl() const override
    {
        return new ArmorComponent(this->armor_class);
    }

public:
    uint8_t armor_class; // formula: (damage*10) / armor_class, chance of
                         // deflection/evasion: armor_class/100;
    ArmorComponent(uint8_t armor_class = 10)
    {
        this->armor_class = armor_class;
    }

    std::unique_ptr<ArmorComponent> clone()
    {
        return std::unique_ptr<ArmorComponent>(this->cloneImpl());
    }
};

#endif /*ARMORCOMPONENT_H*/