#include "critcomponent.h"
#include "../component.h"
#include "buffcomponent.h"
#include <cmath>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

/*debug*/ bool
CritComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto crit = static_cast<const CritComponent *>(c);
    if (*(this->crit_effects) != *(crit->crit_effects))
        return false;
    if (this->crit_chance != crit->crit_chance)
        return false;
    if (std::abs(this->crit_multiplier - crit->crit_multiplier) >=
        0.001) // 0.001 lies inside the acceptable rounding error for me
        return false;

    return true;
}
CritComponent *CritComponent::cloneImpl() const
{
    return new CritComponent(*this);
}

std::ostream &CritComponent::serialize(std::ostream &os) const
{
    os << ComponentType::CRIT << ' ' << +this->crit_chance << ' '
       << this->crit_multiplier << ' ' << *(this->crit_effects);
    return os;
}
std::istream &CritComponent::deserialize(std::istream &is)
{
    ComponentType placeholder;
    int           crit_chance{};
    this->crit_effects = std::make_unique<BuffComponent>();
    is >> crit_chance >> this->crit_multiplier >> placeholder >>
        *(this->crit_effects);
    this->crit_chance = static_cast<uint8_t>(crit_chance);
    return is;
}

CritComponent::CritComponent(uint8_t        crit_chance,
                             double         crit_multiplier,
                             BuffComponent *crit_effects)
{
    this->crit_chance     = crit_chance;
    this->crit_multiplier = crit_multiplier;
    if (crit_effects == nullptr)
    {
        this->crit_effects = std::unique_ptr<BuffComponent>(new BuffComponent);
    }
    else
        this->crit_effects = std::unique_ptr<BuffComponent>(crit_effects);
}

CritComponent::CritComponent(const CritComponent &crit_component)
{
    this->crit_chance     = crit_component.crit_chance;
    this->crit_multiplier = crit_component.crit_multiplier;
    this->crit_effects =
        castToComponent<BuffComponent>(crit_component.crit_effects->clone());
}
ComponentType CritComponent::getType() const { return ComponentType::CRIT; }
