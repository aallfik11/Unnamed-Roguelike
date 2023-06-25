#ifndef EFFECTSYSTEM_H
#define EFFECTSYSTEM_H
#include "../components/buffcomponent.h"
#include "../components/effectcomponent.h"
#include "../effect.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../system.h"
#include <istream>
#include <list>
#include <ostream>
#include <unordered_set>

class EffectSystem : public System, public EntityHolder
{

    std::unordered_set<observer_ptr<Entity>> buffable_entities_;
    std::list<observer_ptr<Entity>>          addition_messages_;
    std::list<observer_ptr<Entity>>          removal_messages_;

public:
    void addEntity(const observer_ptr<Entity> entity);
    void addEntity(std::initializer_list<observer_ptr<Entity>> &entities);

    void removeEntity(const observer_ptr<Entity> entity);

    void applyEffect(const observer_ptr<Entity>          caller,
                     const observer_ptr<EffectComponent> effect_ptr);

    void updateEffects();
    void updateEffects(const observer_ptr<Entity> entity);

    void addEffects(const observer_ptr<Entity>              entity,
                    const observer_ptr<const BuffComponent> buffs_ptr);

    void cleanseEffects(const observer_ptr<Entity>              entity,
                        const observer_ptr<const BuffComponent> effects);
    void cleanseEffects(const observer_ptr<Entity> entity);
    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;
    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*EFFECTSYSTEM_H*/
