#ifndef PLAYER_H
#define PLAYER_H
#include "entity.h"

class Player : Entity
{
    static std::weak_ptr<Player> instance;

public:
    Player() {}
    Player(const Player &) = delete;
    Player(Player &&) = delete;
    void operator=(const Player &) = delete;
    void operator=(Player &&) = delete;

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

    static void destroy_player()
    {
        instance.reset();
    }

    
};

#endif /*PLAYER_H*/