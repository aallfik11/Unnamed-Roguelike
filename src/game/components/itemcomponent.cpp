#include "itemcomponent.h"
#include "../component.h"
#include "../itemtypes.h"
#include "../observerptr.h"
#include "../rarity.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool
ItemComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto i = static_cast<const ItemComponent *>(c);
    return (this->type == i->type && this->stack == i->stack &&
            this->max_stack == i->max_stack && this->rarity == i->rarity &&
            this->equipped == i->equipped);
}
ItemComponent *ItemComponent::cloneImpl() const
{
    return new ItemComponent(*this);
}

std::ostream &ItemComponent::serialize(std::ostream &os) const
{
    os << ComponentType::ITEM << ' ' << this->type << ' ' << this->stack << ' '
       << this->max_stack << ' ' << this->rarity << ' ' << this->equipped
       << ' ';
    return os;
}

std::istream &ItemComponent::deserialize(std::istream &is)
{
    is >> this->type >> this->stack >> this->max_stack >> this->rarity >>
        this->equipped;
    return is;
}

ItemComponent::ItemComponent(ItemType type,
                             uint16_t stack,
                             uint16_t max_stack,
                             Rarity   rarity,
                             bool     equipped)
{
    this->type      = type;
    this->stack     = stack;
    this->max_stack = max_stack;
    this->rarity    = rarity;
    this->equipped  = equipped;
}

ItemComponent::ItemComponent(const ItemComponent &item_component)
{
    this->type      = item_component.type;
    this->stack     = item_component.stack;
    this->max_stack = item_component.max_stack;
    this->rarity    = item_component.rarity;
    this->equipped  = item_component.equipped;
}
ComponentType ItemComponent::getType() const
{
    return ComponentType::INVENTORY;
}
