#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class EffectComponent : public Component
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto e = static_cast<const EffectComponent *>(c);
        return (this->effect == e->effect &&
                this->effect_strength == e->effect_strength &&
                this->effect_duration == e->effect_duration);
    }
    EffectComponent *cloneImpl() const override
    {
        return new EffectComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::EFFECT << ' ' << this->effect << ' '
           << +this->effect_strength << ' ' << this->effect_duration << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        int effect_strength{};
        is >> this->effect >> effect_strength >> this->effect_duration;
        this->effect_strength = static_cast<uint8_t>(effect_strength);

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
                          effect_component.effect_strength,
                          effect_component.effect_duration)
    {
    }
    ComponentType getType() const override { return ComponentType::EFFECT; }
};

#endif /*EFFECTCOMPONENT_H*/
