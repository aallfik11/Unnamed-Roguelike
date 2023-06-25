#ifndef AMULETSLOT_H
#define AMULETSLOT_H
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
class AmuletSlot : public Component, public EntityHolder
{

    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    AmuletSlot *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint8_t                                        amount_equipped;
    uint8_t                                        max_slots;
    std::unordered_set<observer_ptr<const Entity>> amulet_slots;

    AmuletSlot(uint8_t amount_equipped = 0, uint8_t max_slots = 5);
    AmuletSlot(const AmuletSlot &amulet_slot);

    ComponentType getType() const override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*AMULETSLOT_H*/
