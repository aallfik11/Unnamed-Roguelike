#ifndef AMULETSLOT_H
#define AMULETSLOT_H
#include "../component.h"
#include "../entity.h"
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
class AmuletSlot : public Component, public EntityHolder
{

    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto a = static_observer_cast<const AmuletSlot>(c);
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
    AmuletSlot   *cloneImpl() const override { return new AmuletSlot(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::AMULETSLOT << ' ' << +this->amount_equipped << ' '
           << +this->max_slots << ' ';
        for (const auto &amulet : this->amulet_slots)
            os << amulet->getId() << ' ';

        return os;
    }

    std::istream &deserialize(std::istream &is) override
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
                std::make_any<SystemAction::ENTITY>(
                    SystemAction::ENTITY::REQUEST),
                std::make_any<observer_ptr<EntityHolder>>(this),
                std::make_any<std::list<uint32_t>>(entities_requested)};

            System::sendSystemMessage(SystemType::ENTITY, message);
        }
        return is;
    }

public:
    uint8_t                                        amount_equipped;
    uint8_t                                        max_slots;
    std::unordered_set<observer_ptr<const Entity>> amulet_slots;
    AmuletSlot(uint8_t amount_equipped = 0, uint8_t max_slots = 5)
    {
        this->amount_equipped = amount_equipped;
        this->max_slots       = max_slots;
        // might not seem very useful, but this is for the
        // sake of saving and loading a game later
    }

    AmuletSlot(const AmuletSlot &amulet_slot)
    {
        this->amount_equipped = amulet_slot.amount_equipped;
        this->max_slots       = amulet_slot.max_slots;
        for (auto &amulet : amulet_slot.amulet_slots)
        {
            auto new_amulet = new Entity(*amulet);
            auto message    = {
                std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
                std::make_any<observer_ptr<Entity>>(new_amulet)};
            System::sendSystemMessage(SystemType::ENTITY, message);
            this->amulet_slots.emplace(new_amulet);
        }
    }

    ComponentType getType() const override { return ComponentType::AMULETSLOT; }

    void loadEntities(std::list<observer_ptr<Entity>> entities) override
    {
        for (auto entity : entities)
        {
            amulet_slots.insert(static_observer_cast<const Entity>(entity));
        }
    }
};

#endif /*AMULETSLOT_H*/
