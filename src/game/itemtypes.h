#ifndef ITEMTYPES_H
#define ITEMTYPES_H
#include <cstdint>
#include <istream>
#include <ostream>

enum class ItemType : uint16_t
{
    NONE       = 0x0000,
    FOOD       = 0x0001,
    WEAPON     = 0x0002,
    ARMOR      = 0x0004,
    POTION     = 0x0008,
    RING       = 0x0010,

    STACKABLE  = (FOOD | POTION),
    CONSUMABLE = (FOOD | POTION),
    EQUIPABLE  = (WEAPON | ARMOR | RING),
};

inline constexpr ItemType operator&(ItemType effect_1, ItemType effect_2)
{
    return static_cast<ItemType>(static_cast<uint16_t>(effect_1) &
                               static_cast<uint16_t>(effect_2));
}

inline constexpr ItemType operator|(ItemType effect_1, ItemType effect_2)
{
    return static_cast<ItemType>(static_cast<uint16_t>(effect_1) |
                               static_cast<uint16_t>(effect_2));
}

inline constexpr ItemType operator^(ItemType effect_1, ItemType effect_2)
{
    return static_cast<ItemType>(static_cast<uint16_t>(effect_1) ^
                               static_cast<uint16_t>(effect_2));
}

inline constexpr ItemType operator~(ItemType effect_1)
{
    return static_cast<ItemType>(~static_cast<uint16_t>(effect_1));
}

inline ItemType &operator&=(ItemType &effect_1, ItemType effect_2)
{
    effect_1 = effect_1 & effect_2;
    return effect_1;
}

inline ItemType &operator|=(ItemType &effect_1, ItemType effect_2)
{
    effect_1 = effect_1 | effect_2;
    return effect_1;
}

inline ItemType &operator^=(ItemType &effect_1, ItemType effect_2)
{
    effect_1 = effect_1 ^ effect_2;
    return effect_1;
}

inline std::ostream &operator<<(std::ostream &os, const ItemType &type)
{
    os << static_cast<uint16_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, ItemType &type)
{
    uint16_t temp{};
    is >> temp;
    type = static_cast<ItemType>(temp);
    return is;
}

#endif /*ITEMTYPES_H*/
