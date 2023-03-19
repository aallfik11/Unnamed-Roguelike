#ifndef COORDINATES_H
#define COORDINATES_H
#include "../component.h"
#include <cstdint>

class Coordinates : public Component
{
public:
    uint16_t x, y;

    Coordinates(uint16_t x = 0, uint16_t y = 0)
    {
        this->x = x;
        this->y = y;
    }
};

#endif /*COORDINATES_H*/