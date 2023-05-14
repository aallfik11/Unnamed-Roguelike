#ifndef AI_ENUM_H
#define AI_ENUM_H
#include <cstdint>
#include <istream>
#include <ostream>

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

inline std::ostream &operator<<(std::ostream &os, const AIType &type)
{
    os << +static_cast<uint8_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, AIType &type)
{
    int temp;
    is >> temp;
    type = static_cast<AIType>(temp);
    return is;
}

inline std::ostream &operator<<(std::ostream &os, const AIState &type)
{
    os << +static_cast<uint8_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, AIState &type)
{
    int temp;
    is >> temp;
    type = static_cast<AIState>(temp);
    return is;
}
#endif /*AI_ENUM_H*/
