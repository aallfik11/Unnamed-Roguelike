#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include "../tile.h"
#include <ftxui/screen/color.hpp>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

class TileComponent : public Component
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto t = static_cast<const TileComponent *>(c);
        return (this->sprite == t->sprite);
    }
    TileComponent *cloneImpl() const override
    {
        return new TileComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::TILE << ' ' << this->sprite << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->sprite;
        return is;
    }

public:
    TileAppearance sprite;

    TileComponent(TileAppearance sprite = TileAppearance::NONE)
    {
        this->sprite = sprite;
    }

    TileComponent(const TileComponent &tile_component)
    {
        this->sprite = tile_component.sprite;
    }

    ComponentType getType() const override { return ComponentType::TILE; }
};

#endif /*TILECOMPONENT_H*/
