#ifndef HEALTHSYSTEM_H
#define HEALTHSYSTEM_H
#include "../components/health.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../system.h"
#include <cstdint>
#include <list>
#include <memory>
#include <random>
#include <utility>

class HealthSystem : public System
{
    using EntityPtr = std::shared_ptr<Entity>;

    using Message   = std::tuple<EntityPtr, uint16_t, SystemAction::HEALTH>;
    std::list<Message> messages_;

public:
    inline void updateHealth(const EntityPtr     &entity,
                             uint16_t             amount,
                             SystemAction::HEALTH action)
    {
        if (auto health_ptr = entity->getComponent<Health>())
        {
            uint16_t current_health = health_ptr->current_health_points;
            uint16_t max_health     = health_ptr->max_health_points;
            if (action & SystemAction::HEALTH::CURRENT)
            {

                if (action & SystemAction::HEALTH::DAMAGE)
                {
                    if (amount >= current_health)
                    {

                        health_ptr->current_health_points = 0;
                        health_ptr->alive                 = false;
                        entity->type &= ~EntityType::LIVING;
                        entity->type |= EntityType::KILLED;
                    }
                    else
                        health_ptr->current_health_points -= amount;
                }
                else
                {
                    health_ptr->current_health_points =
                        ((amount + current_health) >= max_health)
                            ? max_health
                            : (amount + current_health);
                }
            }
            else
            {
                if (action & SystemAction::HEALTH::DAMAGE)
                {
                    if (amount >= max_health)
                    {
                        health_ptr->max_health_points     = 0;
                        health_ptr->current_health_points = 0;
                    }
                    else
                    {
                        health_ptr->max_health_points = max_health - amount;
                        if (current_health > health_ptr->max_health_points)
                            health_ptr->current_health_points =
                                health_ptr->max_health_points;
                    }
                }
                else
                    health_ptr->max_health_points = max_health + amount;
            }
        }
    }

    virtual void updateData()
    {
        for (auto &message : messages_)
        {
            updateHealth(std::get<0>(message),
                         std::get<1>(message),
                         std::get<2>(message));
        }
    }
    virtual void readSystemMessages()
    {
        for (auto message : (*system_messages_)[SystemType::HEALTH])
        {
            auto message_iterator = message.begin();
            auto entity           = std::any_cast<EntityPtr>(*message_iterator);
            ++message_iterator;
            auto amount = std::any_cast<uint16_t>(*message_iterator);
            ++message_iterator;
            auto action =
                std::any_cast<SystemAction::HEALTH>(*message_iterator);

            messages_.emplace_back(std::make_tuple(entity, amount, action));
        }
    }
    virtual void clearSystemMessages()
    {
        messages_.clear();
        (*system_messages_)[SystemType::HEALTH].clear();
    }
};

#endif /*HEALTHSYSTEM_H*/