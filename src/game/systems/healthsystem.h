#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H
#include "../entity.h"
#include "../system.h"
#include <cstdint>
#include <list>

class HealthSystem : public System
{

    using Message =
        std::tuple<observer_ptr<Entity>, uint16_t, SystemAction::HEALTH>;
    std::list<Message> messages_;

public:
    inline void updateHealth(observer_ptr<Entity> entity,
                             uint16_t             amount,
                             SystemAction::HEALTH action);

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
};

#endif /*HEALTHSYSTEM_H*/
