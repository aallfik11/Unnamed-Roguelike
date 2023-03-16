#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"

class Player : Entity
{
    static std::weak_ptr<Player> instance;
    

public:
    Player(const Player &player) = delete;
    void operator=(const Player &player) = delete;
    static std::shared_ptr<Player> get_instance()
    {
        if (std::shared_ptr<Player> inst_ptr = instance.lock())
        {
            return inst_ptr;
        }
        else
        {
            inst_ptr = std::shared_ptr<Player>(new Player());
            instance = inst_ptr;
            return inst_ptr;
        }
    }
};

// std::weak_ptr<Player> Player::instance = nullptr;

#endif /*PLAYER_H*/