#ifndef CRITCOMPONENT_H
#define CRITCOMPONENT_H
#include "../component.h"
#include "buffcomponent.h"
#include <cmath>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class CritComponent : public Component
{

    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto crit = static_cast<const CritComponent *>(c);
        if (*(this->crit_effects) != *(crit->crit_effects))
            return false;
        if (this->crit_chance != crit->crit_chance)
            return false;
        if (std::abs(this->crit_multiplier - crit->crit_multiplier) >= 0.001) //0.001 lies inside the acceptable rounding error for me
            return false;

        return true;
    }
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
        int           crit_chance{};
        this->crit_effects = std::make_unique<BuffComponent>();
        is >> crit_chance >> this->crit_multiplier >> placeholder >>
            this->crit_effects.get();
        this->crit_chance = static_cast<uint8_t>(crit_chance);
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
    ComponentType getType() const override { return ComponentType::CRIT; }
};

#endif /*CRITCOMPONENT_H*/
