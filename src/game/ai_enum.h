#ifndef AI_ENUM_H
#define AI_ENUM_H
#include <cstdint>

enum AIType : uint8_t
{
    AI_MONSTER_DEFAULT,
    AI_MONSTER_AGGRESSIVE,
    AI_MONSTER_COWARDLY,
};

enum AIState : uint8_t
{
    APPROACH_TARGET,
    RUN_AWAY,
    REST,
    ATTACK,
    INTERACT_WITH_OBJECT,
    WANDER_AROUND,
    SPECIAL
};

#endif /*AI_ENUM_H*/