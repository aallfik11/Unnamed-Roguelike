#ifndef ARMORSLOT_H
#define ARMORSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../system.h"
#include <any>
#include <memory>

class ArmorSlot : public Component
{
    ArmorSlot *cloneImpl() const override { return new ArmorSlot(*this); }

public:
    Entity *armor_item;

    ArmorSlot(Entity *armor_item = nullptr) { this->armor_item = armor_item; }

    ArmorSlot(const ArmorSlot &armor_slot)
    {
        auto entity  = new Entity(*(armor_slot.armor_item));
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
            std::make_any<Entity *>(entity)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        this->armor_item = entity;
    }

    // std::unique_ptr<ArmorSlot> clone() const
    // {
    //     return std::unique_ptr<ArmorSlot>(this->cloneImpl());
    // }
};

#endif /*ARMORSLOT_H*/
