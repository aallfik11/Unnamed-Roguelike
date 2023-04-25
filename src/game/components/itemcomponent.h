#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H
#include "../component.h"
#include "../itemtypes.h"
#include "../rarity.h"
#include <cstdint>
#include <memory>

class ItemComponent : public Component
{
    ItemComponent *cloneImpl() const override
    {
        return new ItemComponent(*this);
    }

public:
    ItemType type;
    uint16_t stack;
    uint16_t max_stack;
    Rarity   rarity;
    bool     equipped;

    ItemComponent(ItemType type,
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
        this->equipped  = item_component.rarity;
    }

    // std::unique_ptr<ItemComponent> clone() const
    // {
    //     return std::unique_ptr<ItemComponent>(this->cloneImpl());
    // }
};

#endif /*ITEMCOMPONENT_H*/
