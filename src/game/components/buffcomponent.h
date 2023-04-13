#ifndef BUFFCOMPONENT_H
#define BUFFCOMPONENT_H
#include "../component.h"
#include "effectcomponent.h"
#include <memory>
#include <unordered_map>

class BuffComponent : public Component
{

public:
    using BuffMap =
        std::unordered_map<Effect, std::shared_ptr<EffectComponent>>;

    BuffMap buffs;

    BuffComponent() {}

    BuffComponent(BuffMap buffs) { this->buffs = buffs; }

    BuffComponent(
        std::initializer_list<std::shared_ptr<EffectComponent>> buffs_list)
    {
        for (auto &buff : buffs_list)
        {
            buffs[(buff->effect & ~(APPLIED | APPLY_ONCE))] = buff;
        }
    }

    BuffComponent(std::initializer_list<EffectComponent *> buffs_list)
    {
        for (auto &buff : buffs_list)
        {
            buffs[(buff->effect & ~(APPLIED | APPLY_ONCE))] =
                std::shared_ptr<EffectComponent>(buff);
        }
    }

    BuffComponent *clone() { return new BuffComponent(this->buffs); }
};

#endif /*BUFFCOMPONENT_H*/