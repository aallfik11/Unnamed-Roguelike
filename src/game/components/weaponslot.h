#ifndef WEAPONSLOT_H
#define WEAPONSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include <any>
#include <istream>
#include <memory>
#include <ostream>

class WeaponSlot : public Component, public EntityHolder
{

    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto w = static_cast<const WeaponSlot *>(c);
        if(this->weapon_item == nullptr && w->weapon_item == nullptr)
        {
            return true;
        }
        return (*(this->weapon_item) == *(w->weapon_item));
    }
    WeaponSlot *cloneImpl() const override
    {
        return new WeaponSlot(this->weapon_item);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::WEAPONSLOT << ' ';
        if (this->weapon_item == nullptr)
        {
            os << 0 << ' ';
        }
        else
        {
            os << this->weapon_item->getId() << ' ';
        }
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
    Entity *weapon_item;

    WeaponSlot(Entity *weapon_item = nullptr)
    {
        this->weapon_item = weapon_item;
    }

    WeaponSlot(const WeaponSlot &weapon_slot)
    {
        auto entity  = new Entity(*(weapon_slot.weapon_item));
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
            std::make_any<Entity *>(entity)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        this->weapon_item = entity;
    }
    ComponentType getType() const override { return ComponentType::WEAPONSLOT; }

    void loadEntities(std::shared_ptr<std::list<Entity *>> &entities) override
    {
        this->weapon_item = *(entities->begin());
    }
};

#endif /*WEAPONSLOT_H*/
