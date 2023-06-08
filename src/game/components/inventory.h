#ifndef INVENTORY_H
#define INVENTORY_H
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

class Inventory : public Component, public EntityHolder
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
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
    Inventory *cloneImpl() const override { return new Inventory(*this); }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::INVENTORY << ' ' << this->inventory.size() << ' ';
        for (auto item : this->inventory)
        {
            os << item->getId() << ' ';
        }
        return os;
    }

    std::istream &deserialize(std::istream &is) override
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
                std::make_any<SystemAction::ENTITY>(
                    SystemAction::ENTITY::REQUEST),
                std::make_any<observer_ptr<EntityHolder>>(this),
                std::make_any<std::list<uint32_t>>(entities_requested)};
            System::sendSystemMessage(SystemType::ENTITY, message);
        }
        return is;
    }

public:
    std::list<Entity *> inventory;

    Inventory(){};

    Inventory(const Inventory &inventory_component)
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
    ComponentType getType() const override { return ComponentType::INVENTORY; }

    void loadEntities(std::list<observer_ptr<Entity>> entities) override
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
};
#endif /*INVENTORY_H*/
