#ifndef INVENTORYSYSTEM_H
#define INVENTORYSYSTEM_H
#include "../components/amuletslot.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/effectcomponent.h"
#include "../components/hungercomponent.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/weaponslot.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../itemtypes.h"
#include "healthsystem.h"
#include <list>

class InventorySystem
{
    using EntityPtr = std::shared_ptr<Entity>;
    using Inv       = std::list<EntityPtr>;

    static inline Inv::iterator iterateToItem(Inv &inventory, uint32_t index)
    {
        auto it = inventory.begin();
        while (it != inventory.end() && index != 0)
        {
            index--;
            it++;
        }
        return it;
    }

    static inline bool stackItem(Inv &inventory, const EntityPtr &item)
    {
        for (auto &inv_item : inventory)
        {
            if (item->getComponent<Name>()->name ==
                inv_item->getComponent<Name>()->name)
            {
                auto item_component = item->getComponent<ItemComponent>();
                auto inv_item_component =
                    inv_item->getComponent<ItemComponent>();

                if (item_component->rarity != inv_item_component->rarity)
                    continue;

                if (inv_item_component->stack == inv_item_component->max_stack)
                    continue;

                inv_item_component->stack++;
                return true;
            }
        }

        return false;
    }

    static inline bool
    equip(EntityPtr    &caller,
          Inv::iterator item_it) // move to private, make inline and
                                 // use in the general "use" function
    {
        auto item           = *item_it;
        auto inventory      = caller->getComponent<Inventory>()->inventory;
        auto item_component = item->getComponent<ItemComponent>();
        auto type           = item_component->type;
        if (type & ItemType::WEAPON)
        {
            auto caller_weaponslot = caller->getComponent<WeaponSlot>();

            // unequipping
            if (caller_weaponslot->weapon_item == item)
            {
                item_component->equipped       = false;
                caller_weaponslot->weapon_item = nullptr;
                return true;
            }

            if (caller_weaponslot->weapon_item != nullptr)
                caller_weaponslot->weapon_item->getComponent<ItemComponent>()
                    ->equipped = false;

            item_component->equipped       = true;
            caller_weaponslot->weapon_item = item;
            return true;
        }
        else if (type & ItemType::ARMOR)
        {
            auto caller_armorslot = caller->getComponent<ArmorSlot>();

            // unequipping
            if (caller_armorslot->armor_item == item)
            {
                item_component->equipped     = false;
                caller_armorslot->armor_item = nullptr;
                return true;
            }

            if (caller_armorslot->armor_item != nullptr)
                caller_armorslot->armor_item->getComponent<ItemComponent>()
                    ->equipped = false;

            item_component->equipped     = true;
            caller_armorslot->armor_item = item;
            return true;
        }
        else if (type & ItemType::RING)
        {
            auto caller_amulets = caller->getComponent<AmuletSlot>();
            if (caller_amulets->amount_equipped == caller_amulets->max_slots)
                return false;

            if (caller_amulets->amulet_slots.contains(item))
            {
                item_component->equipped = false;
                caller_amulets->amulet_slots.erase(item);
                return true;
            }

            caller_amulets->amulet_slots.emplace(item);
            return true;
        }
        return false;
    }

    static inline void consume(EntityPtr &caller, Inv::iterator item_it)
    {
        auto item           = *item_it;
        auto item_component = item->getComponent<ItemComponent>();
        auto type           = item_component->type;

        if (type & ItemType::FOOD)
        {
            auto hunger_component = caller->getComponent<HungerComponent>();
            auto food_component   = item->getComponent<HungerComponent>();
            (food_component->hunger >= hunger_component->max_hunger)
                  ? hunger_component->hunger  = hunger_component->max_hunger
                  : hunger_component->hunger += food_component->hunger;
        }
        if (type & ItemType::POTION)
        {
            // auto effect = item->getComponent<EffectComponent>();
            // // todo: move the most common effects up
            // if (effect->effect & Effect::HEAL)
            // {
            //     uint16_t heal_amount = effect->effect_strength * 10;
            //     HealthSystem::updateHealth(caller, heal_amount, ADD | CURRENT);
            // }
            // if (effect->effect & Effect::POISON)
            // {
            //     caller->getComponent<BuffComponent>()->buffs.emplace_back(
            //         new EffectComponent(Effect::POISON,
            //                             effect->effect_strength,
            //                             effect->effect_duration));
            // }
            // if (effect->effect & Effect::BLEED)
            // {
            //     caller->getComponent<BuffComponent>()->buffs.emplace_back(
            //         new EffectComponent(Effect::BLEED,
            //                             effect->effect_strength,
            //                             effect->effect_duration));
            // }
            // if (effect->effect & Effect::IRONSKIN)
            // {
            //     caller->getComponent<BuffComponent>()->buffs.emplace_back(
            //         new EffectComponent(Effect::IRONSKIN,
            //                             effect->effect_strength,
            //                             effect->effect_duration));
            // }
            // if (effect->effect & Effect::BLIND)
            // {
            //     caller->getComponent<BuffComponent>()->buffs.emplace_back(
            //         new EffectComponent(Effect::BLIND,
            //                             effect->effect_strength,
            //                             effect->effect_duration));
            // }
        }
        if (item_component->stack > 1)
        {
            item_component->stack -= 1;
        }
        else
        {
            caller->getComponent<Inventory>()->inventory.erase(item_it);
        }
    }

public:
    static void addToInventory(Inv &target_inventory,
                               std::initializer_list<EntityPtr> items)
    {
        for (auto &item : items)
        {
            // figure out how to add stackable stuff
            if (item->getComponent<ItemComponent>()->type & ItemType::STACKABLE)
            {
                if (stackItem(target_inventory, item) == true)
                    continue;
            }

            target_inventory.emplace_back(item);
        }
    }

    static EntityPtr dropFromInventory(EntityPtr &caller, uint32_t index)
    {
        EntityPtr item;
        auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
        auto  item_iterator    = iterateToItem(caller_inventory, index);
        if (item_iterator != caller_inventory.end())
        {
            item = *item_iterator;
            caller_inventory.erase(item_iterator);
        }

        auto item_component = item->getComponent<ItemComponent>();
        if (item_component->equipped)
        {
            if (item_component->type & ItemType::ARMOR)
            {
                caller->getComponent<ArmorSlot>()->armor_item = nullptr;
            }
            else if (item_component->type & ItemType::WEAPON)
            {
                caller->getComponent<WeaponSlot>()->weapon_item = nullptr;
            }
            else if (item_component->type & ItemType::RING)
            {
                auto amulet_slot = caller->getComponent<AmuletSlot>();
                amulet_slot->amount_equipped -= 1;
                amulet_slot->amulet_slots.erase(item);
            }

            item_component->equipped = false;
        }

        if (item_component->type & STACKABLE)
        {
            item_component->stack -= 1;
            EntityPtr                      dropped_item(new Entity(item));
            std::shared_ptr<ItemComponent> temp(item_component->clone());
            temp->stack = 1;
            dropped_item->addComponent(temp);
            return dropped_item;
        }

        return item;
    }

    static EntityPtr dropFromInventory(EntityPtr &caller, Inv::iterator &index)
    {
        EntityPtr item;
        auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
        auto  item_iterator    = index;
        if (item_iterator != caller_inventory.end())
        {
            item = *item_iterator;
            caller_inventory.erase(item_iterator);
        }

        auto item_component = item->getComponent<ItemComponent>();
        if (item_component->equipped)
        {
            if (item_component->type & ItemType::ARMOR)
            {
                caller->getComponent<ArmorSlot>()->armor_item = nullptr;
            }
            else if (item_component->type & ItemType::WEAPON)
            {
                caller->getComponent<WeaponSlot>()->weapon_item = nullptr;
            }
            else if (item_component->type & ItemType::RING)
            {
                auto amulet_slot = caller->getComponent<AmuletSlot>();
                amulet_slot->amount_equipped -= 1;
                amulet_slot->amulet_slots.erase(item);
            }

            item_component->equipped = false;
        }

        if (item_component->type & STACKABLE)
        {
            item_component->stack -= 1;
            EntityPtr                      dropped_item(new Entity(item));
            std::shared_ptr<ItemComponent> temp(item_component->clone());
            temp->stack = 1;
            dropped_item->addComponent(temp);
            return dropped_item;
        }

        return item;
    }

    static void useItem(EntityPtr &caller, uint32_t index)
    {
        auto inventory = caller->getComponent<Inventory>()->inventory;
        auto item      = iterateToItem(inventory, index);

        auto item_type = (*item)->getComponent<ItemComponent>()->type;
        if (item_type & ItemType::EQUIPABLE)
        {
            equip(caller, item);
        }
        else if (item_type & ItemType::CONSUMABLE)
        {
            consume(caller, item);
        }
    }

    static void useItem(EntityPtr &caller, Inv::iterator &index)
    {
        auto inventory = caller->getComponent<Inventory>()->inventory;
        // auto item      = index;

        auto item_type = (*index)->getComponent<ItemComponent>()->type;
        if (item_type & ItemType::EQUIPABLE)
        {
            equip(caller, index);
        }
        else if (item_type & ItemType::CONSUMABLE)
        {
            consume(caller, index);
        }
    }
};
#endif /*INVENTORYSYSTEM_H*/