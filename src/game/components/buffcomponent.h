#ifndef BUFFCOMPONENT_H
#define BUFFCOMPONENT_H
#include "../component.h"
#include "../effect.h"
#include "../observerptr.h"
#include "effectcomponent.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>

class BuffComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;
    BuffComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    using BuffMap =
        std::unordered_map<Effect, std::unique_ptr<EffectComponent>>;
    BuffMap buffs;

    BuffComponent() = default;
    BuffComponent(const BuffComponent &buff_component);
    BuffComponent(std::initializer_list<EffectComponent *> buffs_list);

    ComponentType getType() const override;
};

#endif /*BUFFCOMPONENT_H*/
