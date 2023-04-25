#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../action.h"
#include "../ai_enum.h"
#include "../component.h"
#include <cstdint>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>

class AIComponent : public Component
{
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

    std::unique_ptr<Component> deserialize(std::istream &is) override
    {
        AIType   type{};
        AIState  state{};
        uint16_t x{};
        uint16_t y{};

        is >> type >> state >> x >> y;

        return castToComponent<Component, AIComponent>(
            std::make_unique<AIComponent>(type, state, x, y));
    }
};

#endif /*AI_COMPONENT_H*/
