#ifndef ARMORSLOT_H
#define ARMORSLOT_H
#include "../component.h"
#include "../entity.h"
#include <memory>

class ArmorSlot : public Component
{

public:
    std::shared_ptr<Entity> armor_item;

    ArmorSlot(std::shared_ptr<Entity> armor_item = nullptr)
    {
        this->armor_item = armor_item;
    }

    ArmorSlot *clone() { return new ArmorSlot(this->armor_item); }
};

#endif /*ARMORSLOT_H*/