#ifndef LINEOFSIGHTCOMPONENT_H
#define LINEOFSIGHTCOMPONENT_H
#include "../component.h"

class LOSComponent : public Component
{
public:
    // I want to make it work for other entities later on
    bool has_LOS_to_player;
    uint16_t seeingDistance;
    // uint16_t last_player_x;
    // uint16_t last_player_y;

    LOSComponent(uint16_t seeingDistance = 15)
    {
        has_LOS_to_player = false;
        this->seeingDistance = seeingDistance;
    }
};
#endif /*LINEOFSIGHTCOMPONENT_H*/