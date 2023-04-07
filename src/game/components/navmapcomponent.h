#ifndef NAVMAPCOMPONENT_H
#define NAVMAPCOMPONENT_H
#include "../component.h"
#include "../globals.h"
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
public:
    std::vector<std::vector<NavCell>> nav_map;

    NavMapComponent()
    {
        nav_map = std::vector<std::vector<NavCell>>(
            G_MAP_WIDTH, std::vector<NavCell>(G_MAP_HEIGHT));
    }

    NavMapComponent(std::vector<std::vector<NavCell>> nav_map)
    {
        for (auto &col : nav_map)
        {
            this->nav_map.push_back(col);
        }
    }

    NavMapComponent *clone() { return new NavMapComponent(this->nav_map); }
};

#endif /*NAVMAPCOMPONENT_H*/