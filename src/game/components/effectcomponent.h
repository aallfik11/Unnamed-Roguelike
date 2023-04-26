#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class EffectComponent : public Component
{
    EffectComponent *cloneImpl() const override
    {
        return new EffectComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::EFFECT << ' ' << this->effect << ' '
           << this->effect_strength << ' ' << this->effect_duration << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->effect >> this->effect_strength >> this->effect_duration;

        return is;
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

    EffectComponent(const EffectComponent &effect_component)
        : EffectComponent(effect_component.effect,
                          effect_component.effect_duration,
                          effect_component.effect_strength)
    {
    }
};

#endif /*EFFECTCOMPONENT_H*/
