#include "buffcomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include "effectcomponent.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>

/*debug*/ bool
BuffComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto b = static_cast<const BuffComponent *>(c);
    for (auto &[effect, effect_ptr] : this->buffs)
    {
        if (b->buffs.contains(effect) == false)
            return false;
        if (*effect_ptr != *(b->buffs.at(effect)))
            return false;
    }
    return true;
}
BuffComponent *BuffComponent::cloneImpl() const
{
    return new BuffComponent(*this);
}

std::ostream &BuffComponent::serialize(std::ostream &os) const
{
    os << ComponentType::BUFF << ' ' << this->buffs.size() << ' ';
    for (auto &buff : this->buffs)
    {
        os << buff.first << ' ' << *(buff.second);
    }
    return os;
}

std::istream &BuffComponent::deserialize(std::istream &is)
{
    std::size_t buffs_amount;
    is >> buffs_amount;
    this->buffs.reserve(buffs_amount);
    for (std::size_t i = 0; i < buffs_amount; i++)
    {
        Effect key{};
        is >> key;
        ComponentType placeholder;
        is >> placeholder;
        Effect   effect{};
        int      eff_strength{};
        uint16_t eff_duration{};
        is >> effect >> eff_strength >> eff_duration;
        this->buffs[key] = std::make_unique<EffectComponent>(
            effect, static_cast<uint8_t>(eff_strength), eff_duration);
    }
    return is;
}

BuffComponent::BuffComponent(const BuffComponent &buff_component)
{
    for (auto &buff : buff_component.buffs)
    {
        this->buffs[buff.first] =
            castToComponent<EffectComponent>(buff.second->clone());
    }
}

BuffComponent::BuffComponent(
    std::initializer_list<EffectComponent *> buffs_list)
{
    for (auto &buff : buffs_list)
    {
        buffs[(buff->effect & ~(Effect::APPLIED | Effect::APPLY_ONCE))] =
            std::unique_ptr<EffectComponent>(buff);
    }
}

ComponentType BuffComponent::getType() const { return ComponentType::BUFF; }
