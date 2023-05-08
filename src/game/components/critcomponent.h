#ifndef CRITCOMPONENT_H
#define CRITCOMPONENT_H
#include "../component.h"
#include "buffcomponent.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class CritComponent : public Component
{

    CritComponent *cloneImpl() const override
    {
        return new CritComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::CRIT << ' ' << +this->crit_chance << ' '
           << this->crit_multiplier << ' ' << this->crit_effects.get();
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        ComponentType placeholder;
        this->crit_effects = std::make_unique<BuffComponent>();
        is >> this->crit_chance >> this->crit_multiplier >> placeholder >>
            this->crit_effects.get();
        return is;
    }

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
        if (crit_effects == nullptr)
        {
            this->crit_effects =
                std::unique_ptr<BuffComponent>(new BuffComponent);
        }
        else
            this->crit_effects = std::unique_ptr<BuffComponent>(crit_effects);
    }

    CritComponent(const CritComponent &crit_component)
    {
        this->crit_chance     = crit_component.crit_chance;
        this->crit_multiplier = crit_component.crit_multiplier;
        this->crit_effects    = castToComponent<BuffComponent>(
            crit_component.crit_effects->clone());
    }
};

#endif /*CRITCOMPONENT_H*/
