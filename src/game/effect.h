#ifndef EFFECT_H
#define EFFECT_H
#include <cstdint>

enum class Effect : uint16_t // doing this as a bitmask as I might want to
                             // support having multiple effects stacked into one
                             // potion
{
    HEAL     = 0x0001,
    POISON   = 0x0002,
    BLEED    = 0x0004,
    IRONSKIN = 0x0008,
    BLIND    = 0x0010
};

#endif /*EFFECT_H*/