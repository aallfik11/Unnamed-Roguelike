#ifndef PLAYERCONTROLSYSTEM_H
#define PLAYERCONTROLSYSTEM_H
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include "positionsystem.h"
#include <cstdint>
#include <exception>
#include <istream>

class PlayerSystem : public System, public EntityHolder
{
    observer_ptr<Entity>         player_;
    observer_ptr<PositionSystem> pos_sys_;
    Direction                    next_movement_;
    observer_ptr<Entity>         last_hit_enemy_;
    int                          last_hit_entity_timer_;
    bool                        &next_level_;

public:
    PlayerSystem(bool &next_level);
    PlayerSystem(const observer_ptr<Entity>         player,
                 const observer_ptr<PositionSystem> pos_sys,
                 bool                              &next_level);

    void assignPlayer(const observer_ptr<Entity> player);

    void assignPositionSystem(const observer_ptr<PositionSystem> pos_sys);

    void deduceHunger() noexcept(false);

    EntityType checkMovementDestination(
        const std::list<observer_ptr<Entity>> &entities_at_direction);

    observer_ptr<Entity> getLastDamagedEnemy();

    void updateData() noexcept(false) override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    std::istream &deserialize(std::istream &is) override;

    void loadEntities(std::list<observer_ptr<Entity>> player) override;
};

#endif /*PLAYERCONTROLSYSTEM_H*/
