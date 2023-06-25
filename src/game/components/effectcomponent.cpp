#include "effectcomponent.h"
#include "../component.h"
#include "../effect.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
EffectComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto e = static_cast<const EffectComponent *>(c);
    return (this->effect == e->effect &&
            this->effect_strength == e->effect_strength &&
            this->effect_duration == e->effect_duration);
}
EffectComponent *EffectComponent::cloneImpl() const
{
    return new EffectComponent(*this);
}

std::ostream &EffectComponent::serialize(std::ostream &os) const
{
    os << ComponentType::EFFECT << ' ' << this->effect << ' '
       << +this->effect_strength << ' ' << this->effect_duration << ' ';
    return os;
}

std::istream &EffectComponent::deserialize(std::istream &is)
{
    int effect_strength{};
    is >> this->effect >> effect_strength >> this->effect_duration;
    this->effect_strength = static_cast<uint8_t>(effect_strength);

    return is;
}

EffectComponent::EffectComponent(const Effect effect,
                                 uint8_t      effect_strength,
                                 uint16_t     effect_duration)
{
    this->effect          = effect;
    this->effect_strength = effect_strength;
    this->effect_duration = effect_duration;
}

EffectComponent::EffectComponent(const EffectComponent &effect_component)
    : EffectComponent(effect_component.effect,
                      effect_component.effect_strength,
                      effect_component.effect_duration)
{
}
ComponentType EffectComponent::getType() const { return ComponentType::EFFECT; }
