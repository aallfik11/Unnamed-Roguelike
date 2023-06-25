#ifndef LINEOFSIGHTCOMPONENT_H
#define LINEOFSIGHTCOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class LOSComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    LOSComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    // I want to make it work for other entities later on
    bool     has_LOS_to_player;
    uint16_t seeing_distance;

    LOSComponent(bool has_LOS_to_player = false, uint16_t seeing_distance = 15);
    LOSComponent(const LOSComponent &los);

    ComponentType getType() const override;
};
#endif /*LINEOFSIGHTCOMPONENT_H*/
