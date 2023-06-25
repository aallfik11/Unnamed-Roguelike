#include "experiencecomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
ExperienceComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto e = static_cast<const ExperienceComponent *>(c);
    return (this->level == e->level &&
            this->current_experience == e->current_experience);
}
ExperienceComponent *ExperienceComponent::cloneImpl() const
{
    return new ExperienceComponent(*this);
}

std::ostream &ExperienceComponent::serialize(std::ostream &os) const
{
    os << ComponentType::EXPERIENCE << ' ' << +this->level << ' '
       << this->current_experience << ' ';
    return os;
}

std::istream &ExperienceComponent::deserialize(std::istream &is)
{
    int level{};
    is >> level >> this->current_experience;
    this->level = static_cast<uint8_t>(level);
    return is;
}

ExperienceComponent::ExperienceComponent(uint8_t  level,
                                         uint32_t current_experience)
{
    this->level              = level;
    this->current_experience = current_experience;
}

ExperienceComponent::ExperienceComponent(const ExperienceComponent &exp)
{
    this->level              = exp.level;
    this->current_experience = exp.current_experience;
}

ComponentType ExperienceComponent::getType() const
{
    return ComponentType::EXPERIENCE;
}
