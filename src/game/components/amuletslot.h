#ifndef AMULETSLOT_H
#define AMULETSLOT_H
#include "../component.h"
#include "../entity.h"
#include "../system.h"
#include <any>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_set>

/**
 * @brief Holds all amulets equipped by the player
 *
 */
class AmuletSlot : public Component
{
    AmuletSlot   *cloneImpl() const override { return new AmuletSlot(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::AMULETSLOT << ' ' << this->amount_equipped << ' '
           << this->max_slots << ' ';
        for (const auto &amulet : amulet_slots)
            os << amulet->getId() << ' ';
    }

public:
    uint8_t                            amount_equipped;
    uint8_t                            max_slots;
    std::unordered_set<const Entity *> amulet_slots;
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
                std::make_any<Entity *>(new_amulet)};
            System::sendSystemMessage(SystemType::ENTITY, message);
            this->amulet_slots.emplace(new_amulet);
        }
    }

    std::unique_ptr<Component> deserialize(std::istream &is) override
    {
        uint8_t amount_equipped;
        uint8_t max_slots;
        is >> amount_equipped >> max_slots;
        auto temp_slot = std::make_unique<AmuletSlot>(amount_equipped, max_slots);
        //get the entities somehow here:
    }
};

#endif /*AMULETSLOT_H*/
