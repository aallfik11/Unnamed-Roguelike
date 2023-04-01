#ifndef AI_ENUM_H
#define AI_ENUM_H
#include <cstdint>

enum class AIType : uint8_t
{
    AI_MONSTER_DEFAULT,
    AI_MONSTER_AGGRESSIVE,
    AI_MONSTER_COWARDLY,
    AI_MONSTER_BERSERK,
};

enum class AIState : uint8_t
{
    APPROACH_TARGET,
    RUN_AWAY,
    REST,
    ATTACK,
    WANDER_AROUND,
    // INTERACT_WITH_OBJECT,
    // SPECIAL
};

#endif /*AI_ENUM_H*/