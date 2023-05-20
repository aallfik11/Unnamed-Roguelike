#ifndef EXPERIENCESYSTEM_H
#define EXPERIENCESYSTEM_H
#include "../components/experiencecomponent.h"
#include "../components/health.h"
#include "../components/weaponcomponent.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include <list>

class ExperienceSystem : public System, public EntityHolder
{
    observer_ptr<Entity> player_;

    void levelUp()
    {
        auto player_health   = player_->getComponent<Health>();
        auto player_strength = player_->getComponent<WeaponComponent>();
        player_health->max_health_points += 5;
        player_strength->damage          += 2;
    }
    void updateExperience(uint32_t exp)
    {
        auto experience_component =
            player_->getComponent<ExperienceComponent>();
        // formula for exp required is target_level*25
        experience_component->current_experience += exp;
        while (experience_component->current_experience >=
               (experience_component->level + 1) * 25)
        {
            experience_component->current_experience -=
                ((experience_component->level + 1) * 25);
            levelUp();
        }
    }

public:
    void updateData() override {}
    void readSystemMessages() override {}
    void clearSystemMessages() override {}

    void loadEntities(std::list<observer_ptr<Entity>> entities) override {
        player_ = entities.front();
    }
};

#endif /*EXPERIENCESYSTEM_H*/
