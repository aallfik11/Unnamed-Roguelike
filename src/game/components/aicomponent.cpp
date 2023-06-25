#include "aicomponent.h"
#include "../ai_enum.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool AIComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto a = static_cast<const AIComponent *>(c);
    return (this->ai_type == a->ai_type && this->ai_state == a->ai_state &&
            this->last_target_x == a->last_target_x &&
            this->last_target_y == a->last_target_y);
}

AIComponent::AIComponent(const AIComponent &ai_component)
{
    this->ai_type       = ai_component.ai_type;
    this->ai_state      = ai_component.ai_state;
    this->last_target_x = ai_component.last_target_x;
    this->last_target_y = ai_component.last_target_y;
}
AIComponent  *AIComponent::cloneImpl() const { return new AIComponent(*this); }
std::ostream &AIComponent::serialize(std::ostream &os) const
{
    os << ComponentType::AI << ' ' << this->ai_type << ' ' << this->ai_state
       << ' ' << this->last_target_x << ' ' << this->last_target_y << ' ';
    return os;
}
std::istream &AIComponent::deserialize(std::istream &is)
{

    is >> this->ai_type >> this->ai_state >> this->last_target_x >>
        this->last_target_y;
    return is;
}

AIComponent::AIComponent(AIType   ai_type,
                         AIState  ai_state,
                         uint16_t last_target_x,
                         uint16_t last_target_y)
{
    this->ai_type       = ai_type;
    this->ai_state      = ai_state;
    this->last_target_x = last_target_x;
    this->last_target_y = last_target_y;
}

ComponentType AIComponent::getType() const { return ComponentType::AI; }
