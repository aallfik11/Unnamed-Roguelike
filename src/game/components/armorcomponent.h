#ifndef ARMORCOMPONENT_H
#define ARMORCOMPONENT_H
#include "../component.h"
#include <cstdint>

class ArmorComponent : public Component
{

public:
    uint8_t armor_class; // formula: (damage*10) / armor_class, chance of
                         // deflection/evasion: armor_class/100;
    ArmorComponent(uint8_t armor_class) { this->armor_class = armor_class; }

    ArmorComponent *clone() { return new ArmorComponent(this->armor_class); }
};

#endif /*ARMORCOMPONENT_H*/