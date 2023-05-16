#ifndef ARMORSLOT_H
#define ARMORSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include <any>
#include <istream>
#include <list>
#include <memory>
#include <ostream>

class ArmorSlot : public Component, public EntityHolder
{
    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto a = static_cast<const ArmorSlot *>(c);
        if (this->armor_item == nullptr && a->armor_item == nullptr)
            return true;
        return (*(this->armor_item) == *(a->armor_item));
    }
    ArmorSlot    *cloneImpl() const override { return new ArmorSlot(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::ARMORSLOT << ' ';
        if (this->armor_item == nullptr)
        {
            os << 0 << ' ';
        }
        else
            os << this->armor_item->getId() << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        uint32_t temp_entity_id{};
        is >> temp_entity_id;
        if (temp_entity_id != 0)
        {
            std::shared_ptr<std::list<uint32_t>> entities_requested(
                new std::list<uint32_t>);
            entities_requested->push_back(temp_entity_id);
            auto message = {std::make_any<SystemAction::ENTITY>(
                                SystemAction::ENTITY::REQUEST),
                            std::make_any<EntityHolder *>(this),
                            std::make_any<std::shared_ptr<std::list<uint32_t>>>(
                                entities_requested)};
            System::sendSystemMessage(SystemType::ENTITY, message);
        }
        return is;
    }

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

    ComponentType getType() const override { return ComponentType::ARMORSLOT; }
    void loadEntities(std::shared_ptr<std::list<Entity *>> &entities) override
    {
        this->armor_item = entities->front();
    }
};

#endif /*ARMORSLOT_H*/
