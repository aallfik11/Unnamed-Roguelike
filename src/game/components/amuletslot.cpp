#include "amuletslot.h"
#include "../component.h"
#include "../entity.h"
#include "../entityhash.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <any>
#include <cstdint>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <unordered_set>

/**
 * @brief Holds all amulets equipped by the player
 *
 */

/*debug*/ bool AmuletSlot::isEqual(const observer_ptr<const Component> c) const
{
    auto                         a = static_cast<const AmuletSlot *>(c);
    std::unordered_set<uint32_t> this_amulet_ids;
    std::unordered_set<uint32_t> other_amulet_ids;
    for (auto &amulet : this->amulet_slots)
        this_amulet_ids.insert(amulet->getId());
    for (auto &amulet : a->amulet_slots)
        other_amulet_ids.insert(amulet->getId());

    if (this_amulet_ids == other_amulet_ids)
        return true;

    return false;
}

AmuletSlot *AmuletSlot::cloneImpl() const { return new AmuletSlot(*this); }

std::ostream &AmuletSlot::serialize(std::ostream &os) const
{
    os << ComponentType::AMULETSLOT << ' ' << +this->amount_equipped << ' '
       << +this->max_slots << ' ';
    for (const auto &amulet : this->amulet_slots)
        os << amulet->getId() << ' ';

    return os;
}

std::istream &AmuletSlot::deserialize(std::istream &is)
{
    int amount_equipped{}, max_slots{};
    is >> amount_equipped >> max_slots;
    this->amount_equipped = static_cast<uint8_t>(amount_equipped);
    this->max_slots       = static_cast<uint8_t>(max_slots);

    if (amount_equipped != 0)
    {
        std::list<uint32_t> entities_requested;
        for (uint8_t i = 0; i < this->amount_equipped; i++)
        {
            uint32_t temp_id{};
            is >> temp_id;
            entities_requested.push_back(temp_id);
        }
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<EntityHolder *>(this),
            std::make_any<std::list<uint32_t>>(entities_requested)};

        System::sendSystemMessage(SystemType::ENTITY, message);
    }
    return is;
}

AmuletSlot::AmuletSlot(uint8_t amount_equipped, uint8_t max_slots)
{
    this->amount_equipped = amount_equipped;
    this->max_slots       = max_slots;
    // might not seem very useful, but this is for the
    // sake of saving and loading a game later
}

AmuletSlot::AmuletSlot(const AmuletSlot &amulet_slot)
{
    this->amount_equipped = amulet_slot.amount_equipped;
    this->max_slots       = amulet_slot.max_slots;
    for (auto &amulet : amulet_slot.amulet_slots)
    {
        if (amulet == nullptr)
            continue;
        auto new_amulet = new Entity(*amulet);
        auto message    = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
            std::make_any<Entity>(new_amulet)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        this->amulet_slots.emplace(new_amulet);
    }
}

ComponentType AmuletSlot::getType() const { return ComponentType::AMULETSLOT; }

void AmuletSlot::loadEntities(std::list<observer_ptr<Entity>> entities)
{
    for (auto entity : entities)
    {
        amulet_slots.insert(entity);
    }
}
