#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H
#include <cstdint>

enum EntityType : uint16_t
{
    LIVING     = 0x001,
    KILLED     = 0x002,
    UNDEAD     = 0x004,
    ARTIFICIAL = 0x008,

    PLAYER     = 0x010,
    ITEM       = 0x020,
    CREATURE   = 0x040,
    CONTAINER  = 0x080,
};

inline constexpr EntityType operator&(EntityType type_1, EntityType type_2)
{
    return static_cast<EntityType>(static_cast<uint16_t>(type_1) &
                                   static_cast<uint16_t>(type_2));
}

inline constexpr EntityType operator|(EntityType type_1, EntityType type_2)
{
    return static_cast<EntityType>(static_cast<uint16_t>(type_1) |
                                   static_cast<uint16_t>(type_2));
}

inline constexpr EntityType operator^(EntityType type_1, EntityType type_2)
{
    return static_cast<EntityType>(static_cast<uint16_t>(type_1) ^
                                   static_cast<uint16_t>(type_2));
}

inline constexpr EntityType operator~(EntityType type_1)
{
    return static_cast<EntityType>(~static_cast<uint16_t>(type_1));
}

inline EntityType &operator&=(EntityType &type_1, EntityType type_2)
{
    type_1 = type_1 & type_2;
    return type_1;
}

inline EntityType &operator|=(EntityType &type_1, EntityType type_2)
{
    type_1 = type_1 | type_2;
    return type_1;
}

inline EntityType &operator^=(EntityType &type_1, EntityType type_2)
{
    type_1 = type_1 ^ type_2;
    return type_1;
}

#endif /*ENTITYTYPES_H*/