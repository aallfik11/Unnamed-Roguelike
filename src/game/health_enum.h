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

#endif /*HEALTH_ENUM_H*/