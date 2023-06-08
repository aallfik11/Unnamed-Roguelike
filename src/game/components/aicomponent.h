#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../action.h"
#include "../ai_enum.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>

class AIComponent : public Component
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto a = static_cast<const AIComponent*>(c);
        return (this->ai_type == a->ai_type && this->ai_state == a->ai_state &&
                this->last_target_x == a->last_target_x &&
                this->last_target_y == a->last_target_y);
    }

    AIComponent(const AIComponent &ai_component)
    {
        this->ai_type       = ai_component.ai_type;
        this->ai_state      = ai_component.ai_state;
        this->last_target_x = ai_component.last_target_x;
        this->last_target_y = ai_component.last_target_y;
    }
    AIComponent  *cloneImpl() const override { return new AIComponent(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::AI << ' ' << this->ai_type << ' ' << this->ai_state
           << ' ' << this->last_target_x << ' ' << this->last_target_y << ' ';
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {

        is >> this->ai_type >> this->ai_state >> this->last_target_x >>
            this->last_target_y;
        return is;
    }

public:
    AIType   ai_type;
    AIState  ai_state;
    uint16_t last_target_x;
    uint16_t last_target_y;

    AIComponent(AIType   ai_type       = AIType::AI_MONSTER_DEFAULT,
                AIState  ai_state      = AIState::WANDER_AROUND,
                uint16_t last_target_x = 0,
                uint16_t last_target_y = 0)
    {
        this->ai_type       = ai_type;
        this->ai_state      = ai_state;
        this->last_target_x = last_target_x;
        this->last_target_y = last_target_y;
    }

    ComponentType getType() const override { return ComponentType::AI; }
};

#endif /*AI_COMPONENT_H*/
