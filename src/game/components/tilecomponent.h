#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include "../tile.h"
#include <istream>
#include <ostream>

class TileComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    TileComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    TileAppearance sprite;

    TileComponent(TileAppearance sprite = TileAppearance::NONE);
    TileComponent(const TileComponent &tile_component);

    ComponentType getType() const override;
};

#endif /*TILECOMPONENT_H*/
