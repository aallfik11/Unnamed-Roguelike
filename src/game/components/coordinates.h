#ifndef COORDINATES_H
#define COORDINATES_H
#include "../component.h"
#include <cstdint>

class Coordinates : public Component
{
public:
    uint16_t x, y;

    Coordinates()
    {
        x = 0;
        y = 0;
    }
};

#endif /*COORDINATES_H*/