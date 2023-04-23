#ifndef AMULETSLOT_H
#define AMULETSLOT_H
#include "../component.h"
#include "../entity.h"
#include <cstdint>
#include <memory>
#include <unordered_set>

/**
 * @brief Holds all amulets equipped by the player
 *
 */
class AmuletSlot : public Component
{
    AmuletSlot(const AmuletSlot &amulet_slot)
    {
        this->amount_equipped = amulet_slot.amount_equipped;
        this->max_slots       = amulet_slot.max_slots;
        this->amulet_slots    = amulet_slot.amulet_slots;
    }

    AmuletSlot *cloneImpl() const override { return new AmuletSlot(*this); }

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

    std::unique_ptr<AmuletSlot> clone() const
    {
        return std::unique_ptr<AmuletSlot>(this->cloneImpl());
    }
};

#endif /*AMULETSLOT_H*/