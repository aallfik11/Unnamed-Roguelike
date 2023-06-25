#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "../ai_enum.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class AIComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    AIComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    AIType   ai_type;
    AIState  ai_state;
    uint16_t last_target_x;
    uint16_t last_target_y;

    AIComponent(AIType   ai_type       = AIType::AI_MONSTER_DEFAULT,
                AIState  ai_state      = AIState::WANDER_AROUND,
                uint16_t last_target_x = 0,
                uint16_t last_target_y = 0);
    AIComponent(const AIComponent &ai_component);

    ComponentType getType() const override;
};

#endif /*AI_COMPONENT_H*/
