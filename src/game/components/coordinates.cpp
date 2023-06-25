#include "coordinates.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool Coordinates::isEqual(const observer_ptr<const Component> c) const
{
    auto coords = static_cast<const Coordinates *>(c);
    return (this->x == coords->x && this->y == coords->y);
}

Coordinates *Coordinates::cloneImpl() const { return new Coordinates(*this); }

std::ostream &Coordinates::serialize(std::ostream &os) const
{
    os << ComponentType::COORDINATES << ' ' << this->x << ' ' << this->y << ' ';
    return os;
}

std::istream &Coordinates::deserialize(std::istream &is)
{
    is >> this->x >> this->y;
    return is;
}

Coordinates::Coordinates(uint16_t x, uint16_t y)
{
    this->x = x;
    this->y = y;
}

Coordinates::Coordinates(const Coordinates &coordinates)
{
    this->x = coordinates.x;
    this->y = coordinates.y;
}
ComponentType Coordinates::getType() const
{
    return ComponentType::COORDINATES;
}
