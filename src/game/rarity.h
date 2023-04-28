#ifndef RARITY_H
#define RARITY_H
#include <cstdint>
#include <istream>
#include <ostream>

enum class Rarity : uint8_t
{
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
    uint16_t temp{};
    is >> temp;
    type = static_cast<Rarity>(temp);
    return is;
}

#endif /*RARITY_H*/
