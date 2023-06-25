#ifndef EXPERIENCESYSTEM_H
#define EXPERIENCESYSTEM_H
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

    void levelUp(const observer_ptr<Entity> entity) const;
    void updateExperience(const observer_ptr<Entity> entity, uint32_t exp);

public:
    ExperienceSystem();
    ExperienceSystem(observer_ptr<Entity> player);
    void assignPlayer(observer_ptr<Entity> player);

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    std::istream &deserialize(std::istream &is) override;

    void loadEntities(std::list<observer_ptr<Entity>> player) override;
};

#endif /*EXPERIENCESYSTEM_H*/
