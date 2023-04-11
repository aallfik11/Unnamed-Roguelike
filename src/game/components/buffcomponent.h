#ifndef BUFFCOMPONENT_H
#define BUFFCOMPONENT_H
#include "../component.h"
#include "effectcomponent.h"
#include <memory>
#include <unordered_map>

class BuffComponent : public Component
{
public:
    std::unordered_map<Effect, std::shared_ptr<EffectComponent>> buffs;

    BuffComponent() {}

    BuffComponent(
        std::unordered_map<Effect, std::shared_ptr<EffectComponent>> buffs)
    {
        this->buffs = buffs;
    }

    BuffComponent *clone() { return new BuffComponent(this->buffs); }
};

#endif /*BUFFCOMPONENT_H*/