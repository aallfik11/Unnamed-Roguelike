#ifndef MONSTER_H
#define MONSTER_H
#include "entity.h"
#include <array>

class Monster : Entity
{
    using Behavior = std::function<void>;
    std::array<std::array<Behavior, 4>,4> _behavior_table;
    /*
    index 0 - Attack
    index 1 - Approach Player/move
    index 2 - Run Away
    index 4 - Special (can be nothing for common creatures)
    */
   
};

#endif