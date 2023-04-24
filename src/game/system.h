#ifndef SYSTEM_H
#define SYSTEM_H
#include <any>
#include <cstdint>
#include <initializer_list>
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

enum class SystemType
{
    PLAYER,
    AI,
    ATTACK,
    EFFECT,
    ENTITY,
    HEALTH,
    INVENTORY,
    LINE_OF_SIGHT,
    MAP_MANAGER,
    NAVMAP_MANAGER,
    POSITION,
};

namespace SystemAction
{
enum class PLAYER
{
    ADD_TO_MOVEMENT_QUEUE,
    CLEAR_MOVEMENT_QUEUE,
    SYSTEM_TICK_FORWARD,
};
enum class AI
{
};
enum class ATTACK
{
    ATTACK,
};
enum class EFFECT
{
    ADD,
    APPLY,
    CLEANSE,
};
enum class ENTITY
{
    ADD,
    PURGE,
};

enum HEALTH : uint8_t
{
    HEAL    = 0x01,
    DAMAGE  = 0x02,
    CURRENT = 0x04,
    MAX     = 0x08,
    GET     = 0x10,
    UPDATE  = 0x20,

};

enum class INVENTORY
{
    ADD,
    DROP,
    TRANSFER,
    USE,
};
enum class LINE_OF_SIGHT
{
    ADD,
    DELETE,
    CALCULATE,
    CALCULATE_ALL,
};
enum class MAP_MANAGER
{
};
enum class NAVMAP_MANAGER
{
};
enum class POSITION
{
    UPDATE,
    GET,
};
}; // namespace SystemAction

class System
{
public:
    using MessageMap =
        std::unordered_map<SystemType, std::list<std::vector<std::any>>>;

    static std::shared_ptr<MessageMap> system_messages_;

public:
    virtual void updateData()          = 0;
    virtual void readSystemMessages()  = 0;
    virtual void clearSystemMessages() = 0;

    static void
    sendSystemMessage(SystemType                      receiver,
                      std::initializer_list<std::any> message_parameters)
    {
        (*system_messages_)[receiver].emplace_back(message_parameters);
    }
};

std::shared_ptr<System::MessageMap>
    System::system_messages_(new System::MessageMap);

inline constexpr SystemAction::HEALTH operator&(SystemAction::HEALTH health_1,
                                                SystemAction::HEALTH health_2)
{
    return static_cast<SystemAction::HEALTH>(static_cast<uint8_t>(health_1) &
                                             static_cast<uint8_t>(health_2));
}

inline constexpr SystemAction::HEALTH operator|(SystemAction::HEALTH health_1,
                                                SystemAction::HEALTH health_2)
{
    return static_cast<SystemAction::HEALTH>(static_cast<uint8_t>(health_1) |
                                             static_cast<uint8_t>(health_2));
}

inline constexpr SystemAction::HEALTH operator^(SystemAction::HEALTH health_1,
                                                SystemAction::HEALTH health_2)
{
    return static_cast<SystemAction::HEALTH>(static_cast<uint8_t>(health_1) ^
                                             static_cast<uint8_t>(health_2));
}

inline constexpr SystemAction::HEALTH operator~(SystemAction::HEALTH health_1)
{
    return static_cast<SystemAction::HEALTH>(~static_cast<uint8_t>(health_1));
}

inline SystemAction::HEALTH &operator&=(SystemAction::HEALTH &health_1,
                                        SystemAction::HEALTH  health_2)
{
    health_1 = health_1 & health_2;
    return health_1;
}

inline SystemAction::HEALTH &operator|=(SystemAction::HEALTH &health_1,
                                        SystemAction::HEALTH  health_2)
{
    health_1 = health_1 | health_2;
    return health_1;
}

inline SystemAction::HEALTH &operator^=(SystemAction::HEALTH &health_1,
                                        SystemAction::HEALTH  health_2)
{
    health_1 = health_1 ^ health_2;
    return health_1;
}

#endif /*SYSTEM_H*/
