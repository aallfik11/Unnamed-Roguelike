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
        return new ItemComponent(this->type,
                                 this->stack,
                                 this->max_stack,
                                 this->rarity,
                                 this->equipped);
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

    std::unique_ptr<ItemComponent> clone() const
    {
        return std::unique_ptr<ItemComponent>(this->cloneImpl());
    }
};

#endif /*ITEMCOMPONENT_H*/