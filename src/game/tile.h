#ifndef TILE_H
#define TILE_H
#include "entity.h"
#include <cstdint>
#include <memory>
#include <vector>

enum TileType
{
    FLOOR = 0x01,
    WALL = 0x02,
    SPRITE = 0x04,
    TRAVERSIBLE = 0x08,
    CHECKED = 0x10
};

struct Tile
{
    uint8_t type = FLOOR | TRAVERSIBLE;
    std::vector<std::weak_ptr<Entity>> entities;
};

#endif /*TILE_H*/