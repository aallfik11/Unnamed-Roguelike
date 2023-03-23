#ifndef COORDHASH_H
#define COORDHASH_H
#include "components/coordinates.h"
#include <cstdint>

namespace std
{
    template<>
    struct hash<std::pair<uint16_t, uint16_t>>
    {
        std::size_t operator()(const std::pair<uint16_t, uint16_t> &coords) const // this is bad and I won't be using it anyway, but I'll leave it until I finish just in case
        {
            return std::hash<uint64_t>()((uint64_t(coords.first) << 32) | coords.second);
        }
    };
}

#endif /*COORDHASH_H*/