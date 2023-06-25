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

    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    CritComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint8_t                        crit_chance;
    double                         crit_multiplier;
    std::unique_ptr<BuffComponent> crit_effects;

    CritComponent(uint8_t        crit_chance     = 5,
                  double         crit_multiplier = 1.5,
                  BuffComponent *crit_effects    = nullptr);
    CritComponent(const CritComponent &crit_component);

    ComponentType getType() const override;
};

#endif /*CRITCOMPONENT_H*/
