#ifndef TILE_H
#define TILE_H

enum TileType
{
    FLOOR,
    WALL,
    SYMBOL
};

struct Tile
{
    bool traversible = true;
    TileType type = FLOOR;
};

#endif /*TILE_H*/