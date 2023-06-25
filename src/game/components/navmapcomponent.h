#ifndef NAVMAPCOMPONENT_H
#define NAVMAPCOMPONENT_H
#include "../component.h"
#include "../globals.h"
#include "../observerptr.h"
#include <istream>
#include <ostream>
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

class NavMapComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    NavMapComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    std::vector<std::vector<NavCell>> nav_map;

    NavMapComponent();
    NavMapComponent(const std::vector<std::vector<NavCell>> &nav_map);
    NavMapComponent(const NavMapComponent &nav_map_component);
    ComponentType getType() const override;
};

inline constexpr bool operator>(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score > nc2.score);
}

inline constexpr bool operator<(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score < nc2.score);
}

inline constexpr bool operator<=(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score <= nc2.score);
}

inline constexpr bool operator>=(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score >= nc2.score);
}

inline constexpr bool operator==(const NavCell &nc1, const NavCell &nc2)
{
    return (nc1.score == nc2.score && nc1.visited == nc2.visited);
}

inline constexpr bool operator!=(const NavCell &nc1, const NavCell &nc2)
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

#endif /*NAVMAPCOMPONENT_H*/
