#ifndef RARITY_H
#define RARITY_H
#include <cstdint>
#include <istream>
#include <ostream>

enum class Rarity : uint8_t
{
    NONE,
    COMMON,
    UNCOMMON,
    RARE,
    EPIC,
    LEGENDARY
};

inline std::ostream &operator<<(std::ostream &os, const Rarity &type)
{
    os << +static_cast<uint8_t>(type);
    return os;
}

inline std::istream &operator>>(std::istream &is, Rarity &type)
{
    int temp{};
    is >> temp;
    type = static_cast<Rarity>(temp);
    return is;
}

#endif /*RARITY_H*/
