#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H
#include "../component.h"
#include "../itemtypes.h"
#include "../rarity.h"
#include <cstdint>
#include <string>

class ItemComponent : public Component
{
public:
    ItemType type;
    uint16_t stack;
    Rarity   rarity;

    ItemComponent(ItemType type,
                  uint16_t stack  = 1,
                  Rarity   rarity = Rarity::COMMON)
    {
        this->type   = type;
        this->stack  = stack;
        this->rarity = Rarity::COMMON;
    }
};

#endif /*ITEMCOMPONENT_H*/