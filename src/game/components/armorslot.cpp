#include "armorslot.h"
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <any>
#include <istream>
#include <list>
#include <memory>
#include <ostream>

/*debug*/ bool ArmorSlot::isEqual(const observer_ptr<const Component> c) const
{
    auto a = static_cast<const ArmorSlot *>(c);
    if (this->armor_item == nullptr && a->armor_item == nullptr)
        return true;
    return (*(this->armor_item) == *(a->armor_item));
}
ArmorSlot    *ArmorSlot::cloneImpl() const { return new ArmorSlot(*this); }
std::ostream &ArmorSlot::serialize(std::ostream &os) const
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

std::istream &ArmorSlot::deserialize(std::istream &is)
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

ArmorSlot::ArmorSlot(observer_ptr<Entity> armor_item)
{
    this->armor_item = armor_item;
}

ArmorSlot::ArmorSlot(const ArmorSlot &armor_slot)
{
    if (armor_slot.armor_item == nullptr)
    {
        this->armor_item = nullptr;
        return;
    }

    auto entity  = new Entity(*(armor_slot.armor_item));
    auto message = {
        std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
        std::make_any<observer_ptr<Entity>>(entity)};
    System::sendSystemMessage(SystemType::ENTITY, message);
    this->armor_item = entity;
}

ComponentType ArmorSlot::getType() const { return ComponentType::ARMORSLOT; }
void          ArmorSlot::loadEntities(std::list<observer_ptr<Entity>> entities)
{
    this->armor_item = entities.front();
}
