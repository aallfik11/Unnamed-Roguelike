#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H
#include "../component.h"
#include "../itemtypes.h"
#include "../observerptr.h"
#include "../rarity.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class ItemComponent : public Component
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto i = static_cast<const ItemComponent *>(c);
        return (this->type == i->type && this->stack == i->stack &&
                this->max_stack == i->max_stack && this->rarity == i->rarity &&
                this->equipped == i->equipped);
    }
    ItemComponent *cloneImpl() const override
    {
        return new ItemComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::ITEM << ' ' << this->type << ' ' << this->stack
           << ' ' << this->max_stack << ' ' << this->rarity << ' '
           << this->equipped << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->type >> this->stack >> this->max_stack >> this->rarity >>
            this->equipped;
        return is;
    }

public:
    ItemType type;
    uint16_t stack;
    uint16_t max_stack;
    Rarity   rarity;
    bool     equipped;

    ItemComponent(ItemType type      = ItemType::NONE,
                  uint16_t stack     = 1,
                  uint16_t max_stack = 1,
                  Rarity   rarity    = Rarity::COMMON,
                  bool     equipped  = false)
    {
        this->type      = type;
        this->stack     = stack;
        this->max_stack = max_stack;
        this->rarity    = rarity;
        this->equipped  = equipped;
    }

    ItemComponent(const ItemComponent &item_component)
    {
        this->type      = item_component.type;
        this->stack     = item_component.stack;
        this->max_stack = item_component.max_stack;
        this->rarity    = item_component.rarity;
        this->equipped  = item_component.equipped;
    }
    ComponentType getType() const override { return ComponentType::INVENTORY; }
};

#endif /*ITEMCOMPONENT_H*/
