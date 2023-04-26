#ifndef EFFECT_H
#define EFFECT_H
#include <cstdint>
#include <istream>
#include <ostream>

enum Effect : uint16_t // doing this as a bitmask as I might want to
                       // support having multiple effects stacked into one
                       // potion
{
    NONE       = 0x0000,
    HEAL       = 0x0001,
    POISON     = 0x0002,
    BLEED      = 0x0004,
    IRONSKIN   = 0x0008,
    BLIND      = 0x0010,
    STRENGTH   = 0x0020,

    PERMANENT  = 0x1000,
    APPLIED    = 0x2000,
    APPLY_ONCE = 0x4000,
    // in case of things that shouldn't be incremented/applied every turn, like
    // blind or ironskin
};

inline constexpr Effect operator&(Effect effect_1, Effect effect_2)
{
    return static_cast<Effect>(static_cast<uint16_t>(effect_1) &
                               static_cast<uint16_t>(effect_2));
}

inline constexpr Effect operator|(Effect effect_1, Effect effect_2)
{
    return static_cast<Effect>(static_cast<uint16_t>(effect_1) |
                               static_cast<uint16_t>(effect_2));
}

inline constexpr Effect operator^(Effect effect_1, Effect effect_2)
{
    return static_cast<Effect>(static_cast<uint16_t>(effect_1) ^
                               static_cast<uint16_t>(effect_2));
}

inline constexpr Effect operator~(Effect effect_1)
{
    return static_cast<Effect>(~static_cast<uint16_t>(effect_1));
}

inline Effect &operator&=(Effect &effect_1, Effect effect_2)
{
    effect_1 = effect_1 & effect_2;
    return effect_1;
}

inline Effect &operator|=(Effect &effect_1, Effect effect_2)
{
    effect_1 = effect_1 | effect_2;
    return effect_1;
}

inline Effect &operator^=(Effect &effect_1, Effect effect_2)
{
    effect_1 = effect_1 ^ effect_2;
    return effect_1;
}

inline std::ostream &operator<<(std::ostream &os, const Effect &effect)
{
    os << static_cast<uint16_t>(effect);
    return os;
}

inline std::istream &operator>>(std::istream &is, Effect &effect)
{
    uint16_t temp{};
    is >> temp;
    effect = static_cast<Effect>(temp);
    return is;
}

#endif /*EFFECT_H*/
