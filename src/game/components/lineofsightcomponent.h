#ifndef LINEOFSIGHTCOMPONENT_H
#define LINEOFSIGHTCOMPONENT_H
#include "../component.h"
#include <cstdint>

class LOSComponent : public Component
{
public:
    // I want to make it work for other entities later on
    bool     has_LOS_to_player;
    uint16_t seeing_distance;
    // uint16_t last_player_x;
    // uint16_t last_player_y;

    LOSComponent(bool has_LOS_to_player = false, uint16_t seeing_distance = 15)
    {
        this->has_LOS_to_player = has_LOS_to_player;
        this->seeing_distance   = seeing_distance;
    }

    LOSComponent *clone()
    {
        return new LOSComponent(this->has_LOS_to_player, this->seeing_distance);
    }
};
#endif /*LINEOFSIGHTCOMPONENT_H*/