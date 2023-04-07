#ifndef INVENTORYSYSTEM_H
#define INVENTORYSYSTEM_H
#include "../components/effectcomponent.h"
#include "../components/hungercomponent.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../itemtypes.h"
#include <list>

class InventorySystem
{
    using EntityPtr = std::shared_ptr<Entity>;
    using Inv       = std::list<EntityPtr>;

    inline Inv::iterator iterateToItem(Inv &inventory, uint32_t index)
    {
        auto it = inventory.begin();
        while (it != inventory.end() && index != 0)
        {
            index--;
            it++;
        }
        return it;
    }

    inline bool stackItem(Inv &inventory, const EntityPtr &item)
    {
        for (auto &inv_item : inventory)
        {
            if (item->getComponent<Name>()->name ==
                inv_item->getComponent<Name>()->name)
            {
                inv_item->getComponent<ItemComponent>()->stack++;
                return true;
            }
        }

        return false;
    }

    void equip(EntityPtr &caller,
               EntityPtr &item,
               ItemType   type) // move to private, make inline and use in
                              // the general "use" function
    {
        auto inventory = caller->getComponent<Inventory>()->inventory;
        if (type & ItemType::WEAPON)
        {
        }
        else if (type & ItemType::ARMOR)
        {
        }
    }

    void consume(EntityPtr &caller, EntityPtr &item, ItemType type)
    {
        if (type & ItemType::FOOD)
        {
            auto hunger_component = caller->getComponent<HungerComponent>();
            auto food_component   = item->getComponent<HungerComponent>();
            (food_component->hunger >= hunger_component->max_hunger)
                  ? hunger_component->hunger  = hunger_component->max_hunger
                  : hunger_component->hunger += food_component->hunger;
        }
        else if (type & ItemType::POTION)
        {
            auto effect = item->getComponent<EffectComponent>();
            while (effect->effect != Effect::NONE)
            {
                // todo: move the most common effects up
                if (effect->effect & Effect::HEAL)
                {
                    effect->effect &= ~Effect::HEAL;
                }
                if (effect->effect & Effect::POISON)
                {
                    effect->effect &= ~Effect::POISON;
                }
                if (effect->effect & Effect::BLEED)
                {
                    effect->effect &= ~Effect::BLEED;
                }
                if (effect->effect & Effect::IRONSKIN)
                {
                    effect->effect &= ~Effect::IRONSKIN;
                }
                if (effect->effect & Effect::BLIND)
                {
                    effect->effect &= ~Effect::BLIND;
                }
            }
        }
    }

public:
    void addToInventory(Inv                             &target_inventory,
                        std::initializer_list<EntityPtr> items)
    {
        for (auto &item : items)
        {
            // figure out how to add stackable stuff
            if (item->getComponent<ItemComponent>()->type & ItemType::STACKABLE)
            {
                if (stackItem(target_inventory, item) == true)
                    return;
            }

            target_inventory.emplace_back(item);
        }
    }

    EntityPtr dropFromInventory(Inv &caller_inventory, uint32_t index)
    {
        EntityPtr item;
        auto      item_iterator = iterateToItem(caller_inventory, index);
        if (item_iterator != caller_inventory.end())
        {
            caller_inventory.erase(item_iterator);
            item = *item_iterator;
        }
        return item;
    }

    void useItem(EntityPtr &caller, uint32_t index)
    {
        auto inventory = caller->getComponent<Inventory>()->inventory;
        auto item      = iterateToItem(inventory, index);

        auto item_type = (*item)->getComponent<ItemComponent>()->type;
        if (item_type & ItemType::EQUIPABLE)
        {
            equip(caller, *item, item_type);
        }
        else if (item_type & ItemType::CONSUMABLE)
        {
            consume(caller, *item, item_type);
        }
    }
};
#endif /*INVENTORYSYSTEM_H*/