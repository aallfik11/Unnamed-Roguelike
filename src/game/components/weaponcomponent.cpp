#include "weaponcomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
WeaponComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto w = static_cast<const WeaponComponent *>(c);
    return (this->damage == w->damage);
}
WeaponComponent *WeaponComponent::cloneImpl() const
{
    return new WeaponComponent(*this);
}

std::ostream &WeaponComponent::serialize(std::ostream &os) const
{
    os << ComponentType::WEAPON << ' ' << this->damage << ' ';
    return os;
}

std::istream &WeaponComponent::deserialize(std::istream &is)
{
    is >> this->damage;
    return is;
}

uint16_t damage;
WeaponComponent::WeaponComponent(uint16_t damage) { this->damage = damage; }
WeaponComponent::WeaponComponent(const WeaponComponent &weapon_component)
{
    this->damage = weapon_component.damage;
}
ComponentType WeaponComponent::getType() const { return ComponentType::WEAPON; }
