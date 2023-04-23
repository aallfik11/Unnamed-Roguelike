#ifndef NAVMAPCOMPONENT_H
#define NAVMAPCOMPONENT_H
#include "../component.h"
#include "../globals.h"
#include <memory>
#include <ranges>
#include <vector>

struct NavCell        //: std::pair<uint32_t, bool>
{
    uint32_t score;   // = this->first;
    bool     visited; // = this->second;
    NavCell()
    {
        score   = ~0;
        visited = false;
    }
};

bool operator>(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score > nc2.score);
}

bool operator<(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score < nc2.score);
}

bool operator<=(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score <= nc2.score);
}

bool operator>=(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score >= nc2.score);
}

bool operator==(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score == nc2.score && nc1.visited == nc2.visited);
}

bool operator!=(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score != nc2.score || nc1.visited != nc2.visited);
}

class NavMapComponent : public Component
{
    NavMapComponent *cloneImpl() const override
    {
        return new NavMapComponent(this->nav_map);
    }

public:
    std::vector<std::vector<NavCell>> nav_map;

    NavMapComponent()
    {
        nav_map = std::vector<std::vector<NavCell>>(
            G_MAP_WIDTH, std::vector<NavCell>(G_MAP_HEIGHT));
    }

    NavMapComponent(const std::vector<std::vector<NavCell>> &nav_map)
        : NavMapComponent()
    {
        if (nav_map.empty() == false)
        {
            if (nav_map.size() == G_MAP_WIDTH &&
                nav_map.at(0).size() == G_MAP_HEIGHT)
            {
                std::ranges::for_each(std::ranges::views::all(nav_map),
                                      [&](const std::vector<NavCell> &n)
                                      { this->nav_map.emplace_back(n); });
            }
        }
    }

    NavMapComponent(const NavMapComponent &nav_map_component)
        : NavMapComponent(nav_map_component.nav_map)
    {
    }

    std::unique_ptr<NavMapComponent> clone() const
    {
        return std::unique_ptr<NavMapComponent>(this->cloneImpl());
    }
};

#endif /*NAVMAPCOMPONENT_H*/