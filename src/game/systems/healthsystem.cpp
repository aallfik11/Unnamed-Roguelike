#include "healthsystem.h"
#include "../components/experiencecomponent.h"
#include "../components/health.h"
#include "../components/inventory.h"
#include "../entity.h"
#include "../entitytypes.h"
#include "../system.h"
#include <cstdint>
#include <list>
#include <utility>

using Message =
    std::tuple<observer_ptr<Entity>, uint16_t, SystemAction::HEALTH>;

inline void HealthSystem::updateHealth(observer_ptr<Entity> entity,
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

                    health_ptr->current_health_points  = 0;
                    health_ptr->alive                  = false;
                    entity->type                      &= ~EntityType::LIVING;
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
                    health_ptr->max_health_points      = 0;
                    health_ptr->current_health_points  = 0;
                    health_ptr->alive                  = false;
                    entity->type                      &= ~EntityType::LIVING;
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
        if (health_ptr->alive == false &&
            (entity->type & (EntityType::PLAYER | EntityType::KILLED)) ==
                EntityType::NONE)
        {
            entity->type |= EntityType::KILLED;
            sendSystemMessage(
                SystemType::AI,
                {std::make_any<SystemAction::AI>(SystemAction::AI::REMOVE),
                 std::make_any<observer_ptr<Entity>>(entity)});
            sendSystemMessage(SystemType::EXPERIENCE,
                              {std::make_any<SystemAction::EXPERIENCE>(
                                   SystemAction::EXPERIENCE::ADD_TO_PLAYER),
                               std::make_any<uint32_t>(
                                   entity->getComponent<ExperienceComponent>()
                                       ->current_experience)});
            sendSystemMessage(SystemType::POSITION,
                              {std::make_any<SystemAction::POSITION>(
                                   SystemAction::POSITION::DELETE),
                               std::make_any<observer_ptr<Entity>>(entity)});
            if (entity->hasComponent<Inventory>() == true)
            {
                sendSystemMessage(
                    SystemType::INVENTORY,
                    {std::make_any<SystemAction::INVENTORY>(
                         SystemAction::INVENTORY::DROP_ALL),
                     std::make_any<observer_ptr<Entity>>(entity)});
            }
        }
    }
}

void HealthSystem::updateData()
{
    for (auto &message : messages_)
    {
        updateHealth(
            std::get<0>(message), std::get<1>(message), std::get<2>(message));
    }
}
void HealthSystem::readSystemMessages()
{
    for (auto message : (*system_messages_)[SystemType::HEALTH])
    {
        auto message_iterator = message.begin();
        auto entity = std::any_cast<observer_ptr<Entity>>(*message_iterator);
        ++message_iterator;
        auto amount = std::any_cast<uint16_t>(*message_iterator);
        ++message_iterator;
        auto action = std::any_cast<SystemAction::HEALTH>(*message_iterator);

        messages_.emplace_back(std::make_tuple(entity, amount, action));
    }
}
void HealthSystem::clearSystemMessages()
{
    messages_.clear();
    (*system_messages_)[SystemType::HEALTH].clear();
}
