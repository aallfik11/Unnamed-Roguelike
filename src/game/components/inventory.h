#ifndef INVENTORY_H
#define INVENTORY_H
#include "../component.h"
#include "../entity.h"
#include "../system.h"
#include <any>
#include <list>
#include <memory>

class Entity;

class Inventory : public Component
{
    Inventory *cloneImpl() const override { return new Inventory(*this); }

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
                std::make_any<Entity *>(new_item)};
            System::sendSystemMessage(SystemType::ENTITY, message);
            this->inventory.emplace_back(new_item);
        }
    }

    // std::unique_ptr<Inventory> clone() const
    // {
    //     return std::unique_ptr<Inventory>(this->cloneImpl());
    // }
};
#endif /*INVENTORY_H*/
