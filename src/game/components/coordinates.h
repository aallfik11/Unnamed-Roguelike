#ifndef COORDINATES_H
#define COORDINATES_H
#include "../component.h"
#include <cstdint>
#include <memory>

class Coordinates : public Component
{

    Coordinates *cloneImpl() const override
    {
        return new Coordinates(this->x, this->y);
    }

public:
    uint16_t x, y;

    Coordinates(uint16_t x = 0, uint16_t y = 0)
    {
        this->x = x;
        this->y = y;
    }

    std::unique_ptr<Coordinates> clone() const
    {
        return std::unique_ptr<Coordinates>(this->cloneImpl());
    }
};

#endif /*COORDINATES_H*/