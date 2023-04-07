#ifndef ITEMTYPES_H
#define ITEMTYPES_H
#include <cstdint>

enum ItemType : uint16_t
{
    FOOD       = 0x0001,
    WEAPON     = 0x0002,
    ARMOR      = 0x0004,
    POTION     = 0x0008,
    RING       = 0x0010,

    STACKABLE  = (FOOD | POTION),
    CONSUMABLE = (FOOD | POTION),
    EQUIPABLE  = (WEAPON | ARMOR | RING),
};

#endif ITEMTYPES_H