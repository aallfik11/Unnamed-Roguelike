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
#include "../system.h"
#include "effectsystem.h"
#include "healthsystem.h"
#include <list>

class InventorySystem : public System
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

    static inline void
    addEquipmentBuff(std::shared_ptr<BuffComponent> &caller_buffs,
                     std::shared_ptr<BuffComponent> &item_buffs)
    {
        for (auto &item_buff : item_buffs->buffs)
        {
            if (caller_buffs->buffs.contains(item_buff.first))
            {
                caller_buffs->buffs.at(item_buff.first)->effect_strength +=
                    item_buff.second->effect_strength;
            }
            else
                caller_buffs->buffs[item_buff.first] =
                    std::shared_ptr<EffectComponent>(item_buff.second->clone());
        }
    }

    static inline void
    removeEquipmentBuff(std::shared_ptr<BuffComponent> &caller_buffs,
                        std::shared_ptr<BuffComponent> &item_buffs)
    {
        for (auto &item_buff : item_buffs->buffs)
        {
            if (caller_buffs->buffs.at(item_buff.first)->effect_strength ==
                item_buff.second->effect_strength)
            {
                caller_buffs->buffs.erase(item_buff.first);
            }
            else
                caller_buffs->buffs.at(item_buff.first)->effect_strength -=
                    item_buff.second->effect_strength;
        }
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
        auto item_buffs     = item->getComponent<BuffComponent>();
        auto caller_buffs   = caller->getComponent<BuffComponent>();
        auto type           = item_component->type;

        if (type & ItemType::WEAPON)
        {
            auto caller_weaponslot = caller->getComponent<WeaponSlot>();

            // unequipping
            if (caller_weaponslot->weapon_item == item)
            {
                item_component->equipped       = false;
                caller_weaponslot->weapon_item = nullptr;
                if (item_buffs != nullptr)
                {
                    removeEquipmentBuff(caller_buffs, item_buffs);
                }
                return true;
            }

            if (caller_weaponslot->weapon_item != nullptr)
            {
                caller_weaponslot->weapon_item->getComponent<ItemComponent>()
                    ->equipped = false;

                if (auto weapon_buffs = caller_weaponslot->weapon_item
                                            ->getComponent<BuffComponent>())
                {
                    removeEquipmentBuff(caller_buffs, weapon_buffs);
                }
            }

            item_component->equipped       = true;
            caller_weaponslot->weapon_item = item;
            if (item_buffs != nullptr)
            {
                addEquipmentBuff(caller_buffs, item_buffs);
            }
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
                if (item_buffs != nullptr)
                {
                    removeEquipmentBuff(caller_buffs, item_buffs);
                }
                return true;
            }

            if (caller_armorslot->armor_item != nullptr)
            {
                caller_armorslot->armor_item->getComponent<ItemComponent>()
                    ->equipped = false;

                if (auto armor_buffs = caller_armorslot->armor_item
                                           ->getComponent<BuffComponent>())
                {
                    removeEquipmentBuff(caller_buffs, armor_buffs);
                }
            }

            item_component->equipped     = true;
            caller_armorslot->armor_item = item;
            if (item_buffs != nullptr)
            {
                addEquipmentBuff(caller_buffs, item_buffs);
            }
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
                if (item_buffs != nullptr)
                {
                    removeEquipmentBuff(caller_buffs, item_buffs);
                }
                caller_amulets->amulet_slots.erase(item);
                return true;
            }

            caller_amulets->amulet_slots.emplace(item);
            if (item_buffs != nullptr)
            {
                addEquipmentBuff(caller_buffs, item_buffs);
            }
            return true;
        }
        return false;
    }

    static inline void consume(EntityPtr &caller, Inv::iterator item_it)
    {
        auto item           = *item_it;
        auto item_component = item->getComponent<ItemComponent>();

        if (auto food_component = item->getComponent<HungerComponent>())
        {
            auto hunger_component = caller->getComponent<HungerComponent>();

            (food_component->hunger >= hunger_component->max_hunger)
                ? hunger_component->hunger  = hunger_component->max_hunger
                : hunger_component->hunger += food_component->hunger;
        }

        if (auto buff_component = item->getComponent<BuffComponent>())
        {
            auto caller_buffs = caller->getComponent<BuffComponent>();
            for (auto &buff : buff_component->buffs)
            {
                caller_buffs->buffs[buff.first] =
                    std::shared_ptr<EffectComponent>(buff.second->clone());
            }
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
    static void addToInventory(EntityPtr                       &caller,
                               std::initializer_list<EntityPtr> items)
    {
        auto target_inventory = caller->getComponent<Inventory>();
        if (target_inventory == nullptr)
            caller->addComponent(new Inventory());

        for (auto &item : items)
        {
            // figure out how to add stackable stuff
            if (item->getComponent<ItemComponent>()->type & ItemType::STACKABLE)
            {
                if (stackItem(target_inventory->inventory, item) == true)
                    continue;
            }

            target_inventory->inventory.emplace_back(item);
        }
    }

    static EntityPtr dropFromInventory(EntityPtr &caller, uint32_t index)
    {
        // EntityPtr item;
        auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
        auto  item_iterator    = caller_inventory.begin();
        std::advance(item_iterator, index);
        return dropFromInventory(caller, item_iterator);
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
        auto inventory     = caller->getComponent<Inventory>()->inventory;
        auto item_iterator = inventory.begin();
        std::advance(item_iterator, index);
        useItem(caller, item_iterator);
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

    void updateData()
    {
        for (auto &message : (*system_messages_)[SystemType::INVENTORY])
        {
            auto message_iterator = message.begin();
            auto action =
                std::any_cast<SystemAction::INVENTORY>(*message_iterator);
            ++message_iterator;
            auto entity = std::any_cast<EntityPtr>(*message_iterator);
            ++message_iterator;
            switch (action)
            {
            case SystemAction::INVENTORY::ADD:
            {
                auto items = std::any_cast<std::initializer_list<EntityPtr>>(
                    *message_iterator);
                addToInventory(entity, items);
                break;
            }
            case SystemAction::INVENTORY::DROP:
            {
                auto index = std::any_cast<uint32_t>(*message_iterator);
                dropFromInventory(entity, index);
                break;
            }
            case SystemAction::INVENTORY::TRANSFER:
            {
                auto index = std::any_cast<uint32_t>(*message_iterator);
                ++message_iterator;
                auto target = std::any_cast<EntityPtr>(*message_iterator);
                addToInventory(target, {dropFromInventory(entity, index)});
                break;
            }
            case SystemAction::INVENTORY::USE:
            {
                auto index = std::any_cast<uint32_t>(*message_iterator);
                useItem(entity, index);
                break;
            }
            }
        }
    }
    void readSystemMessages() {}
    void clearSystemMessages()
    {
        (*system_messages_)[SystemType::INVENTORY].clear();
    }
};
#endif /*INVENTORYSYSTEM_H*/