#ifndef HEALTH_ENUM_H
#define HEALTH_ENUM_H
#include <cstdint>

enum HealthAction : uint8_t
{
    ADD     = 0x01,
    DEDUCE  = 0x02,
    MAX     = 0x04,
    CURRENT = 0x08
};

inline constexpr HealthAction operator&(HealthAction health_1,
                                        HealthAction health_2)
{
    return static_cast<HealthAction>(static_cast<uint8_t>(health_1) &
                                     static_cast<uint8_t>(health_2));
}

inline constexpr HealthAction operator|(HealthAction health_1,
                                        HealthAction health_2)
{
    return static_cast<HealthAction>(static_cast<uint8_t>(health_1) |
                                     static_cast<uint8_t>(health_2));
}

inline constexpr HealthAction operator^(HealthAction health_1,
                                        HealthAction health_2)
{
    return static_cast<HealthAction>(static_cast<uint8_t>(health_1) ^
                                     static_cast<uint8_t>(health_2));
}

inline constexpr HealthAction operator~(HealthAction health_1)
{
    return static_cast<HealthAction>(~static_cast<uint8_t>(health_1));
}

inline HealthAction &operator&=(HealthAction &health_1, HealthAction health_2)
{
    health_1 = health_1 & health_2;
    return health_1;
}

inline HealthAction &operator|=(HealthAction &health_1, HealthAction health_2)
{
    health_1 = health_1 | health_2;
    return health_1;
}

inline HealthAction &operator^=(HealthAction &health_1, HealthAction health_2)
{
    health_1 = health_1 ^ health_2;
    return health_1;
}

#endif /*HEALTH_ENUM_H*/