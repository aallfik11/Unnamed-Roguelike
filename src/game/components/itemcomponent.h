#ifndef ITEMCOMPONENT_H
#define ITEMCOMPONENT_H
#include "../component.h"
#include "../itemtypes.h"
#include "../observerptr.h"
#include "../rarity.h"
#include <cstdint>
#include <istream>
#include <ostream>

class ItemComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    ItemComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

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
                  bool     equipped  = false);
    ItemComponent(const ItemComponent &item_component);

    ComponentType getType() const override;
};

#endif /*ITEMCOMPONENT_H*/
