#include "../component.h"
#include "../observerptr.h"
#include "armorcomponent.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
ArmorComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto a = static_cast<const ArmorComponent *>(c);
    return (this->armor_class == a->armor_class);
}
ArmorComponent *ArmorComponent::cloneImpl() const
{
    return new ArmorComponent(*this);
}

std::ostream &ArmorComponent::serialize(std::ostream &os) const
{
    os << ComponentType::ARMOR << ' ' << +this->armor_class << ' ';
    return os;
}

std::istream &ArmorComponent::deserialize(std::istream &is)
{
    int armor_class{};
    is >> armor_class;
    this->armor_class = static_cast<uint8_t>(armor_class);
    return is;
}

ArmorComponent::ArmorComponent(uint8_t armor_class)
{
    this->armor_class = armor_class;
}

ArmorComponent::ArmorComponent(const ArmorComponent &armor_component)
{
    this->armor_class = armor_component.armor_class;
}
ComponentType ArmorComponent::getType() const { return ComponentType::ARMOR; }
