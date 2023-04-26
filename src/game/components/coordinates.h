#ifndef COORDINATES_H
#define COORDINATES_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class Coordinates : public Component
{

    Coordinates *cloneImpl() const override { return new Coordinates(*this); }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::COORDINATES << ' ' << this->x << ' ' << this->y
           << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->x >> this->y;
        return is;
    }

public:
    uint16_t x, y;

    Coordinates(uint16_t x = 0, uint16_t y = 0)
    {
        this->x = x;
        this->y = y;
    }

    Coordinates(const Coordinates &coordinates)
    {
        this->x = coordinates.x;
        this->y = coordinates.y;
    }
};

#endif /*COORDINATES_H*/
