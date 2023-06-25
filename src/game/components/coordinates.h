#ifndef COORDINATES_H
#define COORDINATES_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class Coordinates : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    Coordinates *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint16_t x, y;

    Coordinates(uint16_t x = 0, uint16_t y = 0);
    Coordinates(const Coordinates &coordinates);

    ComponentType getType() const override;
};

#endif /*COORDINATES_H*/
