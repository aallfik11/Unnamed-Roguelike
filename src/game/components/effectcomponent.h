#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include <cstdint>
#include <memory>

class EffectComponent : public Component
{

    EffectComponent *cloneImpl() const override
    {
        return new EffectComponent(
            this->effect, this->effect_strength, this->effect_duration);
    }

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

    std::unique_ptr<EffectComponent> clone() const
    {
        return std::unique_ptr<EffectComponent>(this->cloneImpl());
    }
};

#endif /*EFFECTCOMPONENT_H*/