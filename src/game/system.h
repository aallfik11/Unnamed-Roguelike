#ifndef SYSTEM_H
#define SYSTEM_H
#include <any>
#include <cstdint>
#include <initializer_list>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
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
    EXPERIENCE,
    LOG,
};

namespace SystemAction
{
enum class PLAYER
{
    ENTITIES_UP,
    ENTITIES_DOWN,
    ENTITIES_LEFT,
    ENTITIES_RIGHT,
    MOVE,
};
enum class AI
{
    ADD,
    REMOVE,
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
    ADD_ENTITY,
    REMOVE_ENTITY,
};
enum class ENTITY
{
    ADD,
    PURGE,
    REQUEST,
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
    DROP_ALL,
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
    ADD,
    REMOVE,
};
enum class POSITION
{
    UPDATE,
    GET,
    ADD,
    DELETE,
    RECEIVE_PLAYER_COORDINATES,
};
enum class EXPERIENCE
{
    ADD,
    ADD_TO_PLAYER,
    FORCE_LEVEL_UP,
};
enum class LOG
{
    ADD_TO_LOGS,
};
}; // namespace SystemAction


class System
{
    protected:
    using MessageMap =
        std::unordered_map<SystemType, std::list<std::vector<std::any>>>;

    static std::shared_ptr<MessageMap> system_messages_;

public:
    virtual void          updateData()          = 0;
    virtual void          readSystemMessages()  = 0;
    virtual void          clearSystemMessages() = 0;
    virtual std::ostream &serialize(std::ostream &os) const { return os; };
    virtual std::istream &deserialize(std::istream &is) { return is; };

    static void sendSystemMessage(SystemType                      receiver,
                                  std::initializer_list<std::any> message)
    {
        (*system_messages_)[receiver].emplace_back(message);
    }

    friend std::ostream &operator<<(std::ostream &os, const System &system)
    {
        return system.serialize(os);
    }
    friend std::istream &operator>>(std::istream &is, System &system)
    {
        return system.deserialize(is);
    }
};

std::shared_ptr<System::MessageMap>
    System::system_messages_(new System::MessageMap);

inline std::ostream &operator<<(std::ostream &os, const SystemType &s)
{
    os << static_cast<int>(s);
    return os;
}

inline std::istream &operator>>(std::istream &is, SystemType &s)
{
    int temp{};
    is >> temp;
    s = static_cast<SystemType>(temp);
    return is;
}

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
