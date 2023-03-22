#ifndef COORDHASH_H
#define COORDHASH_H
#include "components/coordinates.h"
#include <memory>

namespace std
{
    struct hash<std::weak_ptr<Coordinates>>
    {
        std::size_t operator()(const std::weak_ptr<Coordinates> &coords_ptr) const //this is bad and I won't be using it anyway, but I'll leave it until I finish just in case
        {
            if (auto coords = coords_ptr.lock())
                return std::hash<uint64_t>()((uint64_t(coords->x) << 32) | coords->y);
            else
                return -1;
        }
    };
}

#endif /*COORDHASH_H*/