#include "inventorysystem.h"
#include "../components/amuletslot.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/coordinates.h"
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
#include <istream>
#include <list>
#include <ostream>

using Inv = std::list<observer_ptr<Entity>>;

inline Inv::iterator InventorySystem::iterateToItem(Inv     &inventory,
                                                    uint32_t index)
{
    auto it = inventory.begin();
    while (it != inventory.end() && index != 0)
    {
        index--;
        it++;
    }
    return it;
}

inline void InventorySystem::addEquipmentBuff(
    const observer_ptr<BuffComponent>       caller_buffs,
    const observer_ptr<const BuffComponent> item_buffs)
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

inline void InventorySystem::removeEquipmentBuff(
    const observer_ptr<BuffComponent>       caller_buffs,
    const observer_ptr<const BuffComponent> item_buffs)
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

inline bool InventorySystem::stackItem(Inv                       &inventory,
                                       const observer_ptr<Entity> item)
{
    for (auto &inv_item : inventory)
    {
        if (item->getComponent<Name>()->name ==
            inv_item->getComponent<Name>()->name)
        {
            auto item_component     = item->getComponent<ItemComponent>();
            auto inv_item_component = inv_item->getComponent<ItemComponent>();

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

inline bool InventorySystem::equip(const observer_ptr<Entity> caller,
                                   const Inv::iterator       &item_it)
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

            if (auto armor_buffs =
                    caller_armorslot->armor_item->getComponent<BuffComponent>())
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

        if (caller_amulets->amulet_slots.contains(item))
        {
            item_component->equipped = false;
            if (item_buffs != nullptr)
            {
                removeEquipmentBuff(caller_buffs, item_buffs);
            }
            caller_amulets->amulet_slots.erase(item);
            caller_amulets->amount_equipped -= 1;
            return true;
        }
        if (caller_amulets->amount_equipped == caller_amulets->max_slots)
            return false;

        caller_amulets->amulet_slots.emplace(item);
        item_component->equipped = true;
        if (item_buffs != nullptr)
        {
            addEquipmentBuff(caller_buffs, item_buffs);
        }
        caller_amulets->amount_equipped += 1;
        return true;
    }
    return false;
}

inline void
InventorySystem::InventorySystem::consume(const observer_ptr<Entity> caller,
                                          const Inv::iterator       &item_it)
{
    auto item           = *item_it;
    auto item_component = item->getComponent<ItemComponent>();

    if (auto food_component = item->getComponent<HungerComponent>())
    {
        auto hunger_component = caller->getComponent<HungerComponent>();

        (food_component->hunger + hunger_component->hunger >=
         hunger_component->max_hunger)
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
        auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
        caller_inventory.erase(item_it);
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::PURGE),
             std::make_any<uint32_t>(item->getId())});
    }
}

void InventorySystem::addToInventory(
    const observer_ptr<Entity>                  caller,
    std::initializer_list<observer_ptr<Entity>> items)
{
    auto target_inventory = caller->getComponent<Inventory>();
    if (target_inventory == nullptr)
        caller->addComponent(new Inventory());

    for (auto &item : items)
    {
        // figure out how to add stackable stuff
        if ((item->getComponent<ItemComponent>()->type & ItemType::STACKABLE) !=
            ItemType::NONE)
        {
            if (stackItem(target_inventory->inventory, item) == true)
                continue;
        }

        target_inventory->inventory.emplace_back(item);
    }
}

void InventorySystem::addToInventory(
    const observer_ptr<Entity>             caller,
    const std::list<observer_ptr<Entity>> &items)
{
    auto target_inventory = caller->getComponent<Inventory>();
    if (target_inventory == nullptr)
        caller->addComponent(new Inventory());

    for (auto &item : items)
    {
        // figure out how to add stackable stuff
        if ((item->getComponent<ItemComponent>()->type & ItemType::STACKABLE) !=
            ItemType::NONE)
        {
            if (stackItem(target_inventory->inventory, item) == true)
                continue;
        }

        target_inventory->inventory.emplace_back(item);
    }
}

observer_ptr<Entity>
InventorySystem::dropFromInventory(const observer_ptr<Entity> caller,
                                   const uint32_t             index,
                                   const bool                 add_to_pos)
{
    // EntityPtr item;
    auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
    if (caller_inventory.empty())
    {
        return nullptr;
    }
    auto item_iterator = caller_inventory.begin();
    std::advance(item_iterator, index);
    return dropFromInventory(caller, item_iterator, add_to_pos);
}

observer_ptr<Entity>
InventorySystem::dropFromInventory(const observer_ptr<Entity> caller,
                                   const Inv::iterator       &index,
                                   const bool                 add_to_pos)
{
    auto &caller_inventory = caller->getComponent<Inventory>()->inventory;
    if (index == caller_inventory.end())
    {
        return nullptr;
    }
    observer_ptr<Entity> item          = *index;
    auto                 item_iterator = index;

    auto item_component                = item->getComponent<ItemComponent>();
    if (item_component->equipped)
    {
        if ((item_component->type & ItemType::ARMOR) != ItemType::NONE)
        {
            caller->getComponent<ArmorSlot>()->armor_item = nullptr;
        }
        else if ((item_component->type & ItemType::WEAPON) != ItemType::NONE)
        {
            caller->getComponent<WeaponSlot>()->weapon_item = nullptr;
        }
        else if ((item_component->type & ItemType::RING) != ItemType::NONE)
        {
            auto amulet_slot              = caller->getComponent<AmuletSlot>();
            amulet_slot->amount_equipped -= 1;
            amulet_slot->amulet_slots.erase(item);
            if (auto item_buffs = item->getComponent<BuffComponent>())
            {
                removeEquipmentBuff(caller->getComponent<BuffComponent>(),
                                    item_buffs);
            }
        }

        item_component->equipped = false;
    }

    if ((item_component->type & ItemType::STACKABLE) != ItemType::NONE &&
        item_component->stack > 1)
    {
        item_component->stack                              -= 1;
        auto dropped_item                                   = new Entity(*item);
        dropped_item->getComponent<ItemComponent>()->stack  = 1;
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
             std::make_any<observer_ptr<Entity>>(dropped_item)});

        item = dropped_item;
    }
    else
    {

        if (item_iterator != caller_inventory.end())
        {
            caller_inventory.erase(item_iterator);
        }
    }
    if (add_to_pos == true)
    {
        item->removeComponent<Coordinates>();
        item->addComponent(caller->getComponent<Coordinates>()->clone());
        sendSystemMessage(
            SystemType::POSITION,
            {std::make_any<SystemAction::POSITION>(SystemAction::POSITION::ADD),
             std::make_any<observer_ptr<Entity>>(item)});
    }

    return item;
}

void InventorySystem::dropAllItems(observer_ptr<Entity> caller)
{
    auto caller_coords = caller->getComponent<Coordinates>();
    for (auto &item : caller->getComponent<Inventory>()->inventory)
    {
        auto item_component = item->getComponent<ItemComponent>();

        if (item_component->equipped == true)
        {
            item_component->equipped = false;
        }
        for (int i = 0; i < item_component->stack; ++i)
        {
            auto dropped_item = new Entity(*item);
            dropped_item->getComponent<ItemComponent>()->stack = 1;
            sendSystemMessage(
                SystemType::ENTITY,
                {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
                 std::make_any<observer_ptr<Entity>>(dropped_item)});

            if (dropped_item->hasComponent<Coordinates>())
            {
                auto coords = dropped_item->getComponent<Coordinates>();
                coords->x   = caller_coords->x;
                coords->y   = caller_coords->y;
            }
            else
                dropped_item->addComponent(caller_coords->clone());

            sendSystemMessage(
                SystemType::POSITION,
                {std::make_any<SystemAction::POSITION>(
                     SystemAction::POSITION::ADD),
                 std::make_any<observer_ptr<Entity>>(dropped_item)});
        }
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::PURGE),
             std::make_any<uint32_t>(item->getId())});
    }
    caller->getComponent<Inventory>()->inventory.clear();
    return;
}

void InventorySystem::useItem(const observer_ptr<Entity> caller,
                              const uint32_t             index)
{
    if (index < 0)
        return;
    auto &inventory = caller->getComponent<Inventory>()->inventory;
    if (inventory.empty())
        return;

    auto item_iterator = inventory.begin();
    std::advance(item_iterator, index);
    useItem(caller, item_iterator);
}

void InventorySystem::useItem(const observer_ptr<Entity> caller,
                              const Inv::iterator       &index)
{
    auto &inventory = caller->getComponent<Inventory>()->inventory;
    if (index == inventory.end())
    {
        return;
    }
    auto item_type = (*index)->getComponent<ItemComponent>()->type;
    if ((item_type & ItemType::EQUIPABLE) != ItemType::NONE)
    {
        equip(caller, index);
    }
    else if ((item_type & ItemType::CONSUMABLE) != ItemType::NONE)
    {
        consume(caller, index);
    }
}

void InventorySystem::updateData()
{
    for (auto &[caller, items] : addition_messages_)
    {
        addToInventory(caller, items);
    }

    for (auto &[caller, index] : usage_messages_)
    {
        useItem(caller, index);
    }

    for (auto &[caller, item] : transfer_messages_)
    {
        addToInventory(caller, {item});
    }

    for (auto &[caller, index] : drop_messages_)
    {
        dropFromInventory(caller, index);
    }
}
void InventorySystem::readSystemMessages()
{
    for (auto &message : (*system_messages_)[SystemType::INVENTORY])
    {
        auto message_iterator = message.begin();
        auto action = std::any_cast<SystemAction::INVENTORY>(*message_iterator);
        ++message_iterator;
        auto entity = std::any_cast<observer_ptr<Entity>>(*message_iterator);
        ++message_iterator;
        switch (action)
        {
        case SystemAction::INVENTORY::ADD:
        {
            auto items = std::any_cast<std::list<observer_ptr<Entity>>>(
                *message_iterator);
            addition_messages_.emplace_back(entity, items);
            break;
        }
        case SystemAction::INVENTORY::DROP:
        {
            auto index = std::any_cast<uint32_t>(*message_iterator);
            drop_messages_.emplace_back(entity, index);
            break;
        }
        case SystemAction::INVENTORY::DROP_ALL:
        {
            dropAllItems(entity);
            break;
        }
        case SystemAction::INVENTORY::TRANSFER:
        {
            auto index = std::any_cast<uint32_t>(*message_iterator);
            ++message_iterator;
            auto target =
                std::any_cast<observer_ptr<Entity>>(*message_iterator);
            transfer_messages_.emplace_back(
                target, dropFromInventory(entity, index, false));
            break;
        }
        case SystemAction::INVENTORY::USE:
        {
            auto index = std::any_cast<uint32_t>(*message_iterator);
            usage_messages_.emplace_back(entity, index);
            break;
        }
        }
    }
}
void InventorySystem::clearSystemMessages()
{
    (*system_messages_)[SystemType::INVENTORY].clear();
    addition_messages_.clear();
    usage_messages_.clear();
    transfer_messages_.clear();
    drop_messages_.clear();
}
