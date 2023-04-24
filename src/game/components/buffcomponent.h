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
        std::unordered_map<Effect, std::unique_ptr<EffectComponent>>;
    BuffMap buffs;

    BuffComponent() {}

    BuffComponent(const BuffComponent &buff_component)
    {
        for (auto &buff : buff_component.buffs)
        {
            this->buffs[buff.first] =
                std::unique_ptr<EffectComponent>(buff.second->clone());
        }
    }

    BuffComponent(std::initializer_list<EffectComponent *> buffs_list)
    {
        for (auto &buff : buffs_list)
        {
            buffs[(buff->effect & ~(APPLIED | APPLY_ONCE))] =
                std::unique_ptr<EffectComponent>(buff);
        }
    }

    // std::unique_ptr<Component> clone() const override
    // {
    //     return std::unique_ptr<BuffComponent>(this->cloneImpl());
    // }

    BuffComponent *clone() const override { return new BuffComponent(*this); }
};

#endif /*BUFFCOMPONENT_H*/