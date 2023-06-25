#ifndef EFFECTCOMPONENT_H
#define EFFECTCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class EffectComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    EffectComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    Effect   effect;
    uint8_t  effect_strength;
    uint16_t effect_duration;

    EffectComponent(const Effect effect          = Effect::NONE,
                    uint8_t      effect_strength = 1,
                    uint16_t     effect_duration = 1);
    EffectComponent(const EffectComponent &effect_component);

    ComponentType getType() const override;
};

#endif /*EFFECTCOMPONENT_H*/
