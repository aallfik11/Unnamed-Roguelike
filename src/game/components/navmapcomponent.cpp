#include "navmapcomponent.h"
#include "../component.h"
#include "../globals.h"
#include "../observerptr.h"
#include <istream>
#include <ostream>
#include <vector>
/*debug*/ bool NavMapComponent::isEqual(const observer_ptr<const Component> c) const
{
    auto n = static_cast<const NavMapComponent *>(c);
    for (std::size_t i = 0; i < this->nav_map.size(); ++i)
    {
        if (this->nav_map[i] != n->nav_map[i])
            return false;
    }
    return true;
}
NavMapComponent *NavMapComponent::cloneImpl() const { return new NavMapComponent(*this); }

std::ostream &NavMapComponent::serialize(std::ostream &os) const
{
    os << ComponentType::NAVMAP << ' ';
    for (auto &col : this->nav_map)
    {
        for (auto &cell : col)
        {
            os << cell << ' ';
        }
    }
    return os;
}

std::istream &NavMapComponent::deserialize(std::istream &is)
{
    for (auto &col : this->nav_map)
    {
        for (auto &cell : col)
        {
            is >> cell;
        }
    }
    return is;
}

NavMapComponent::NavMapComponent()
{
    nav_map = std::vector<std::vector<NavCell>>(
        G_MAP_WIDTH, std::vector<NavCell>(G_MAP_HEIGHT));
}

NavMapComponent::NavMapComponent(const std::vector<std::vector<NavCell>> &nav_map)
    : NavMapComponent()
{
    if (nav_map.empty() == false)
    {
        if (nav_map.size() == G_MAP_WIDTH &&
            nav_map.at(0).size() == G_MAP_HEIGHT)
        {
            this->nav_map = nav_map;
        }
    }
}

NavMapComponent::NavMapComponent(const NavMapComponent &nav_map_component)
    : NavMapComponent(nav_map_component.nav_map)
{
}
ComponentType NavMapComponent::getType() const { return ComponentType::NAVMAP; }
