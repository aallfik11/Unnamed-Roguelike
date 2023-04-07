#ifndef BUFFCOMPONENT_H
#define BUFFCOMPONENT_H
#include "../component.h"
#include "effectcomponent.h"
#include <list>
#include <memory>

class BuffComponent : public Component
{
public:
    std::list<std::shared_ptr<EffectComponent>> buffs;

    BuffComponent() {}

    BuffComponent(std::list<std::shared_ptr<EffectComponent>> buffs)
    {
        this->buffs = buffs;
    }

    BuffComponent *clone() { return new BuffComponent(this->buffs); }
};

#endif /*BUFFCOMPONENT_H*/