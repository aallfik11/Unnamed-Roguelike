#include "weaponslot.h"
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <any>
#include <istream>
#include <memory>
#include <ostream>

/*debug*/ bool WeaponSlot::isEqual(const observer_ptr<const Component> c) const
{
    auto w = static_cast<const WeaponSlot *>(c);
    if (this->weapon_item == nullptr && w->weapon_item == nullptr)
    {
        return true;
    }
    return (*(this->weapon_item) == *(w->weapon_item));
}
WeaponSlot *WeaponSlot::cloneImpl() const { return new WeaponSlot(*this); }

std::ostream &WeaponSlot::serialize(std::ostream &os) const
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

std::istream &WeaponSlot::deserialize(std::istream &is)
{
    uint32_t temp_entity_id{};
    is >> temp_entity_id;
    if (temp_entity_id != 0)
    {
        std::list<uint32_t> entities_requested;
        entities_requested.push_back(temp_entity_id);
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<observer_ptr<EntityHolder>>(this),
            std::make_any<std::list<uint32_t>>(entities_requested)};
        System::sendSystemMessage(SystemType::ENTITY, message);
    }
    return is;
}

WeaponSlot::WeaponSlot(observer_ptr<Entity> weapon_item)
{
    this->weapon_item = weapon_item;
}

WeaponSlot::WeaponSlot(const WeaponSlot &weapon_slot)
{
    if (weapon_slot.weapon_item == nullptr)
    {
        this->weapon_item = nullptr;
        return;
    }

    auto entity  = new Entity(*(weapon_slot.weapon_item));
    auto message = {
        std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
        std::make_any<observer_ptr<Entity>>(entity)};
    System::sendSystemMessage(SystemType::ENTITY, message);
    this->weapon_item = entity;
}

ComponentType WeaponSlot::getType() const { return ComponentType::WEAPONSLOT; }

void WeaponSlot::loadEntities(std::list<observer_ptr<Entity>> entities)
{
    this->weapon_item = *entities.begin();
}
