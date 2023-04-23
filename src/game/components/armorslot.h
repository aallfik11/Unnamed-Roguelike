#ifndef ARMORSLOT_H
#define ARMORSLOT_H
#include "../component.h"
#include "../entity.h"
#include <memory>

class ArmorSlot : public Component
{

    ArmorSlot *cloneImpl() const override
    {
        return new ArmorSlot(this->armor_item);
    }

public:
    Entity *armor_item;

    ArmorSlot(Entity *armor_item = nullptr) { this->armor_item = armor_item; }

    std::unique_ptr<ArmorSlot> clone() const
    {
        return std::unique_ptr<ArmorSlot>(this->cloneImpl());
    }
};

#endif /*ARMORSLOT_H*/