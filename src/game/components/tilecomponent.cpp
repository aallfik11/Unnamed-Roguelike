#include "tilecomponent.h"
#include "../component.h"
#include "../observerptr.h"
#include "../tile.h"
#include <istream>
#include <ostream>

/*debug*/ bool
TileComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto t = static_cast<const TileComponent *>(c);
    return (this->sprite == t->sprite);
}

TileComponent *TileComponent::cloneImpl() const
{
    return new TileComponent(*this);
}

std::ostream &TileComponent::serialize(std::ostream &os) const
{
    os << ComponentType::TILE << ' ' << this->sprite << ' ';
    return os;
}

std::istream &TileComponent::deserialize(std::istream &is)
{
    is >> this->sprite;
    return is;
}

TileComponent::TileComponent(TileAppearance sprite) { this->sprite = sprite; }

TileComponent::TileComponent(const TileComponent &tile_component)
{
    this->sprite = tile_component.sprite;
}

ComponentType TileComponent::getType() const { return ComponentType::TILE; }
