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
#include <istream>
#include <list>
#include <ostream>

class InventorySystem : public System
{

    using Inv = std::list<Entity *>;

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

    inline void addEquipmentBuff(BuffComponent *const       caller_buffs,
                                 const BuffComponent *const item_buffs)
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
                    castToComponent<EffectComponent>(item_buff.second->clone());
        }
    }

    inline void removeEquipmentBuff(BuffComponent *const       caller_buffs,
                                    const BuffComponent *const item_buffs)
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

    inline bool stackItem(Inv &inventory, Entity *const item)
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

    inline bool
    equip(Entity *const        caller,
          const Inv::iterator &item_it) // move to private, make inline and
                                        // use in the general "use" function
    {
        auto item           = *item_it;
        auto inventory      = caller->getComponent<Inventory>()->inventory;
        auto item_component = item->getComponent<ItemComponent>();
        auto item_buffs     = item->getComponent<BuffComponent>();
        auto caller_buffs   = caller->getComponent<BuffComponent>();
        auto type           = item_component->type;

        if ((type & ItemType::WEAPON) != ItemType::NONE)
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
        else if ((type & ItemType::ARMOR) != ItemType::NONE)
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
        else if ((type & ItemType::RING) != ItemType::NONE)
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

    inline void consume(Entity *const caller, const Inv::iterator &item_it)
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
                    castToComponent<EffectComponent>(buff.second->clone());
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
    void addToInventory(Entity *const                   caller,
                        std::initializer_list<Entity *> items)
    {
        auto target_inventory = caller->getComponent<Inventory>();
        if (target_inventory == nullptr)
            caller->addComponent(new Inventory());

        for (auto &item : items)
        {
            // figure out how to add stackable stuff
            if ((item->getComponent<ItemComponent>()->type &
                 ItemType::STACKABLE) != ItemType::NONE)
            {
                if (stackItem(target_inventory->inventory, item) == true)
                    continue;
            }

            target_inventory->inventory.emplace_back(item);
        }
    }

    Entity *dropFromInventory(Entity *const caller, const uint32_t index)
    {
        // EntityPtr item;
        auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
        auto  item_iterator    = caller_inventory.begin();
        std::advance(item_iterator, index);
        return dropFromInventory(caller, item_iterator);
    }

    Entity *dropFromInventory(Entity *const caller, const Inv::iterator &index)
    {
        Entity *item             = nullptr;
        auto   &caller_inventory = caller->getComponent<Inventory>()->inventory;
        auto    item_iterator    = index;
        if (item_iterator != caller_inventory.end())
        {
            item = *item_iterator;
            caller_inventory.erase(item_iterator);
        }

        auto item_component = item->getComponent<ItemComponent>();
        if (item_component->equipped)
        {
            if ((item_component->type & ItemType::ARMOR) != ItemType::NONE)
            {
                caller->getComponent<ArmorSlot>()->armor_item = nullptr;
            }
            else if ((item_component->type & ItemType::WEAPON) !=
                     ItemType::NONE)
            {
                caller->getComponent<WeaponSlot>()->weapon_item = nullptr;
            }
            else if ((item_component->type & ItemType::RING) != ItemType::NONE)
            {
                auto amulet_slot = caller->getComponent<AmuletSlot>();
                amulet_slot->amount_equipped -= 1;
                amulet_slot->amulet_slots.erase(item);
            }

            item_component->equipped = false;
        }

        if ((item_component->type & ItemType::STACKABLE) != ItemType::NONE)
        {
            item_component->stack -= 1;
            Entity *dropped_item(new Entity(*item));
            dropped_item->getComponent<ItemComponent>()->stack = 1;
            sendSystemMessage(
                SystemType::ENTITY,
                {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
                 std::make_any<Entity *>(dropped_item)});

            item = dropped_item;
        }

        return item;
    }

    void useItem(Entity *const caller, const uint32_t index)
    {
        auto inventory     = caller->getComponent<Inventory>()->inventory;
        auto item_iterator = inventory.begin();
        std::advance(item_iterator, index);
        useItem(caller, item_iterator);
    }

    void useItem(Entity *const caller, const Inv::iterator &index)
    {
        auto inventory = caller->getComponent<Inventory>()->inventory;
        // auto item      = index;

        auto item_type = (*index)->getComponent<ItemComponent>()->type;
        if ((item_type & ItemType::EQUIPABLE) != ItemType::NONE)
        {
            equip(caller, index);
        }
        else if ((item_type & ItemType::CONSUMABLE) != ItemType::NONE)
            ;
        {
            consume(caller, index);
        }
    }

    void updateData() override
    {
        for (auto &message : (*system_messages_)[SystemType::INVENTORY])
        {
            auto message_iterator = message.begin();
            auto action =
                std::any_cast<SystemAction::INVENTORY>(*message_iterator);
            ++message_iterator;
            auto entity = std::any_cast<Entity *>(*message_iterator);
            ++message_iterator;
            switch (action)
            {
            case SystemAction::INVENTORY::ADD:
            {
                auto items = std::any_cast<std::initializer_list<Entity *>>(
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
                auto target = std::any_cast<Entity *>(*message_iterator);
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
    void readSystemMessages() override {}
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::INVENTORY].clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::INVENTORY;
        return os;
    }
    std::istream &deserialize(std::istream &is) override { return is; }
};
#endif /*INVENTORYSYSTEM_H*/
