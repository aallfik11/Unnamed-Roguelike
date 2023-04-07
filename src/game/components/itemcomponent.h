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
    uint16_t max_stack;
    Rarity   rarity;
    bool     equipped;

    ItemComponent(ItemType type,
                  uint16_t stack     = 1,
                  uint16_t max_stack = 1,
                  Rarity   rarity    = Rarity::COMMON,
                  bool     equipped  = false)
    {
        this->type      = type;
        this->stack     = stack;
        this->max_stack = max_stack;
        this->rarity    = rarity;
        this->equipped  = equipped;
    }
};

#endif /*ITEMCOMPONENT_H*/