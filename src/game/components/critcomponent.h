#ifndef CRITCOMPONENT_H
#define CRITCOMPONENT_H
#include "../component.h"
#include "buffcomponent.h"
#include <cstdint>
#include <memory>

class CritComponent : public Component
{

public:
    uint8_t                        crit_chance;
    double                         crit_multiplier;
    std::unique_ptr<BuffComponent> crit_effects;

    CritComponent(uint8_t        crit_chance     = 5,
                  double         crit_multiplier = 1.5,
                  BuffComponent *crit_effects    = nullptr)
    {
        this->crit_chance     = crit_chance;
        this->crit_multiplier = crit_multiplier;
        this->crit_effects    = std::unique_ptr<BuffComponent>(crit_effects);
    }

    CritComponent(const CritComponent &crit_component)
    {
        this->crit_chance     = crit_component.crit_chance;
        this->crit_multiplier = crit_component.crit_multiplier;
        this->crit_effects    = std::unique_ptr<BuffComponent>(
            crit_component.crit_effects->clone());
    }

    CritComponent *clone() const override { return new CritComponent(*this); }
    // std::unique_ptr<CritComponent> clone() const
    // {
    //     return std::unique_ptr<CritComponent>(this->cloneImpl());
    // }
};

#endif /*CRITCOMPONENT_H*/
