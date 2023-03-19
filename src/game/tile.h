#ifndef TILE_H
#define TILE_H
#include <cstdint>

enum TileType
{
    FLOOR = 0x01,
    WALL = 0x02,
    SYMBOL = 0x04,
    TRAVERSIBLE = 0x08,
    CHECKED = 0x10
};

struct Tile
{
    uint_least8_t type = FLOOR | TRAVERSIBLE;
};

#endif /*TILE_H*/