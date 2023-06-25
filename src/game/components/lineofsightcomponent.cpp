#include "lineofsightcomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
LOSComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto l = static_cast<const LOSComponent *>(c);
    return (this->has_LOS_to_player == l->has_LOS_to_player &&
            this->seeing_distance == l->seeing_distance);
}
LOSComponent *LOSComponent::cloneImpl() const
{
    return new LOSComponent(*this);
}

std::ostream &LOSComponent::serialize(std::ostream &os) const
{
    os << ComponentType::LINEOFSIGHT << ' ' << this->has_LOS_to_player << ' '
       << this->seeing_distance << ' ';
    return os;
}

std::istream &LOSComponent::deserialize(std::istream &is)
{
    is >> this->has_LOS_to_player >> this->seeing_distance;
    return is;
}

LOSComponent::LOSComponent(bool has_LOS_to_player, uint16_t seeing_distance)
{
    this->has_LOS_to_player = has_LOS_to_player;
    this->seeing_distance   = seeing_distance;
}

LOSComponent::LOSComponent(const LOSComponent &los)
{
    this->has_LOS_to_player = los.has_LOS_to_player;
    this->seeing_distance   = los.seeing_distance;
}
ComponentType LOSComponent::getType() const
{
    return ComponentType::LINEOFSIGHT;
}
