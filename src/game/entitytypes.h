#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H
#include <cstdint>

enum EntityType : uint16_t
{
    LIVING     = 0x001,
    DEAD       = 0x002,
    UNDEAD     = 0x004,
    ARTIFICIAL = 0x008,

    PLAYER     = 0x010,
    ITEM       = 0x020,
    CREATURE   = 0x040,
    CONTAINER  = 0x080,
};

#endif /*ENTITYTYPES_H*/