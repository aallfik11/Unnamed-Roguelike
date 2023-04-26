#ifndef NAVMAPCOMPONENT_H
#define NAVMAPCOMPONENT_H
#include "../component.h"
#include "../globals.h"
#include <algorithm>
#include <istream>
#include <memory>
#include <ostream>
#include <ranges>
#include <vector>

struct NavCell //: std::pair<uint32_t, bool>
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

inline std::ostream &operator<<(std::ostream &os, const NavCell &n)
{
    os << n.score << ' ' << n.visited;
    return os;
}

inline std::istream &operator>>(std::istream &is, NavCell &n)
{
    is >> n.score >> n.visited;
    return is;
}

class NavMapComponent : public Component
{
    NavMapComponent *cloneImpl() const override
    {
        return new NavMapComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
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

    std::istream &deserialize(std::istream &is) override
    {
        std::ranges::for_each(std::ranges::views::all(this->nav_map),
                              [&](std::vector<NavCell> &n)
                              {
                                  std::ranges::for_each(
                                      std::ranges::views::all(n),
                                      [&](NavCell &nav) { is >> nav; });
                              });
        return is;
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
};

#endif /*NAVMAPCOMPONENT_H*/
