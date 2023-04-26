#ifndef LINEOFSIGHTCOMPONENT_H
#define LINEOFSIGHTCOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class LOSComponent : public Component
{
    LOSComponent *cloneImpl() const override { return new LOSComponent(*this); }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::LINEOFSIGHT << ' ' << this->has_LOS_to_player
           << ' ' << this->seeing_distance << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->has_LOS_to_player >> this->seeing_distance;
        return is;
    }

public:
    // I want to make it work for other entities later on
    bool     has_LOS_to_player;
    uint16_t seeing_distance;

    LOSComponent(bool has_LOS_to_player = false, uint16_t seeing_distance = 15)
    {
        this->has_LOS_to_player = has_LOS_to_player;
        this->seeing_distance   = seeing_distance;
    }

    LOSComponent(const LOSComponent &los)
    {
        this->has_LOS_to_player = los.has_LOS_to_player;
        this->seeing_distance   = los.seeing_distance;
    }
};
#endif /*LINEOFSIGHTCOMPONENT_H*/
