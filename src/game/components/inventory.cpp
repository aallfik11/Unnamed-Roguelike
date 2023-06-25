#include "inventory.h"
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <any>
#include <istream>
#include <list>
#include <ostream>

/*debug*/ bool Inventory::isEqual(const observer_ptr<const Component> c) const
{
    auto i = static_cast<const Inventory *>(c);
    for (auto &entity : this->inventory)
    {
        bool found = false;
        for (auto &other_entity : i->inventory)
        {
            if (*entity == *other_entity)
            {
                found = true;
                break;
            }
        }
        if (found == false)
            return false;
    }
    return true;
}
Inventory *Inventory::cloneImpl() const { return new Inventory(*this); }

std::ostream &Inventory::serialize(std::ostream &os) const
{
    os << ComponentType::INVENTORY << ' ' << this->inventory.size() << ' ';
    for (auto item : this->inventory)
    {
        os << item->getId() << ' ';
    }
    return os;
}

std::istream &Inventory::deserialize(std::istream &is)
{
    std::size_t inv_size{};
    is >> inv_size;
    if (inv_size != 0)
    {
        uint32_t            temp_entity_id{};
        std::list<uint32_t> entities_requested;
        for (std::size_t i = 0; i < inv_size; i++)
        {
            is >> temp_entity_id;
            entities_requested.emplace_back(temp_entity_id);
        }
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<observer_ptr<EntityHolder>>(this),
            std::make_any<std::list<uint32_t>>(entities_requested)};
        System::sendSystemMessage(SystemType::ENTITY, message);
    }
    return is;
}

Inventory::Inventory(const Inventory &inventory_component)
{
    for (auto &item : inventory_component.inventory)
    {
        auto new_item = new Entity(*item);
        auto message  = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
            std::make_any<observer_ptr<Entity>>(new_item)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        this->inventory.emplace_back(new_item);
    }
}
ComponentType Inventory::getType() const { return ComponentType::INVENTORY; }

void Inventory::loadEntities(std::list<observer_ptr<Entity>> entities)
{
    if (inventory.empty() == false)
    {
        inventory.clear();
    }
    for (auto &entity : entities)
    {
        this->inventory.emplace_back(entity);
    }
}
