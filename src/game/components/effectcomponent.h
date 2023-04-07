#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include <cstdint>

class EffectComponent : public Component
{
public:
    Effect  effect;
    uint8_t effect_strength;
    uint8_t effect_duration; // in turns

    EffectComponent(Effect  effect          = Effect::NONE,
                    uint8_t effect_strength = 1,
                    uint8_t effect_duration = 0)
    {
        this->effect          = effect;
        this->effect_strength = effect_strength;
        this->effect_duration = effect_duration;
    }

    EffectComponent *clone()
    {
        return new EffectComponent(
            this->effect, this->effect_strength, this->effect_duration);
    }
};

#endif /*EFFECTCOMPONENT_H*/