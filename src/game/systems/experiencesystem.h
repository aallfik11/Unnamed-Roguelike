#ifndef EXPERIENCESYSTEM_H
#define EXPERIENCESYSTEM_H
#include "../components/experiencecomponent.h"
#include "../components/health.h"
#include "../components/weaponcomponent.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../observerptr.h"
#include "../system.h"
#include <cstdint>
#include <istream>
#include <list>
#include <ostream>

class ExperienceSystem : public System, public EntityHolder
{

    std::list<std::pair<observer_ptr<Entity>, uint32_t>> update_messages_;
    std::list<observer_ptr<Entity>>                      force_lvl_ups;
    observer_ptr<Entity>                                 player_;

    void levelUp(const observer_ptr<Entity> entity) const
    {
        auto entity_health   = entity->getComponent<Health>();
        auto entity_strength = entity->getComponent<WeaponComponent>();
        // magic numbers as of rn, I'll try coming up with something better
        // later
        entity_health->max_health_points                   += 5;
        entity_strength->damage                            += 2;
        entity->getComponent<ExperienceComponent>()->level += 1;
    }
    void updateExperience(const observer_ptr<Entity> entity, uint32_t exp)
    {
        auto experience_component = entity->getComponent<ExperienceComponent>();
        // formula for exp required is target_level*25
        experience_component->current_experience += exp;
        while (experience_component->current_experience >=
               (experience_component->level + 1) * 25)
        {
            experience_component->current_experience -=
                ((experience_component->level + 1) * 25);
            levelUp(entity);
        }
    }

public:
    ExperienceSystem() { player_ = nullptr; }
    ExperienceSystem(observer_ptr<Entity> player) : player_{player} {}

    void assignPlayer(observer_ptr<Entity> player) { this->player_ = player; }

    void updateData() override
    {
        for (auto &[entity, amount] : update_messages_)
        {
            updateExperience(entity, amount);
        }
        for (auto &entity : force_lvl_ups)
        {
            levelUp(entity);
        }
    }

    void readSystemMessages() override
    {
        if (player_ == nullptr)
            throw std::runtime_error(
                "Experience System: ERROR -> Player unassigned");
        for (auto &message : (*system_messages_)[SystemType::EXPERIENCE])
        {
            auto message_it = message.begin();
            auto action = std::any_cast<SystemAction::EXPERIENCE>(*message_it);
            ++message_it;
            switch (action)
            {
            case SystemAction::EXPERIENCE::ADD:
            {
                auto entity = std::any_cast<observer_ptr<Entity>>(*message_it);
                ++message_it;
                auto xp_amount = std::any_cast<uint32_t>(*message_it);
                update_messages_.emplace_back(entity, xp_amount);
                break;
            }
            case SystemAction::EXPERIENCE::ADD_TO_PLAYER:
            {
                auto xp_amount = std::any_cast<uint32_t>(*message_it);
                update_messages_.emplace_back(player_, xp_amount);
                break;
            }
            case SystemAction::EXPERIENCE::FORCE_LEVEL_UP:
            {
                auto entity = std::any_cast<observer_ptr<Entity>>(*message_it);
                force_lvl_ups.emplace_back(entity);
                break;
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::EXPERIENCE].clear();
        update_messages_.clear();
        force_lvl_ups.clear();
    }

    void resetSystem() override
    {
        clearSystemMessages();
        player_ = nullptr;
    }

    std::istream &deserialize(std::istream &is) override
    {
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
             std::make_any<observer_ptr<EntityHolder>>(this),
             std::make_any<std::list<uint32_t>>(std::list<uint32_t>({1}))});
        return is;
    }

    void loadEntities(std::list<observer_ptr<Entity>> player) override
    {
        player_ = player.front();
    }
};

#endif /*EXPERIENCESYSTEM_H*/
