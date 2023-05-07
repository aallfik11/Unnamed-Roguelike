#ifndef ENTITYTYPES_H
#define ENTITYTYPES_H
#include <cstdint>
#include <istream>
#include <ostream>

enum class EntityType : uint16_t
{
    NONE       = 0x000,
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

inline std::ostream &operator<<(std::ostream &os, const EntityType &type)
{
    os << static_cast<uint16_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, EntityType &type)
{
    uint16_t temp{};
    is >> temp;
    type = static_cast<EntityType>(temp);
    return is;
}

#endif /*ENTITYTYPES_H*/
