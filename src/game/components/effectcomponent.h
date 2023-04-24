#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include <cstdint>
#include <memory>

class EffectComponent : public Component
{

public:
    Effect   effect;
    uint8_t  effect_strength;
    uint16_t effect_duration;

    EffectComponent(const Effect effect          = Effect::NONE,
                    uint8_t      effect_strength = 1,
                    uint16_t     effect_duration = 1)
    {
        this->effect          = effect;
        this->effect_strength = effect_strength;
        this->effect_duration = effect_duration;
    }

    EffectComponent(const EffectComponent &effect_component)
        : EffectComponent(effect_component.effect,
                          effect_component.effect_duration,
                          effect_component.effect_strength)
    {
    }
    // std::unique_ptr<Component> clone() const override
    // {
    //     return std::unique_ptr<EffectComponent>(this->cloneImpl());
    // }

    EffectComponent *clone() const override
    {
        return new EffectComponent(*this);
    }
};

#endif /*EFFECTCOMPONENT_H*/
