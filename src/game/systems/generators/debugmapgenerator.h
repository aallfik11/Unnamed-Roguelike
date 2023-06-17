#ifndef DEBUGMAPGENERATOR_H
#define DEBUGMAPGENERATOR_H
#include "../../tile.h"
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

class DebugMapGenerator
{
    using GameMap = std::vector<std::vector<Tile>>;

public:
    static GameMap generate(std::mt19937 &, uint32_t size_x, uint32_t size_y)
    {
        Tile floor;
        Tile wall;
        wall.type       = TileType::WALL;
        wall.appearance = TileAppearance::WALL;
        std::vector<Tile> top_and_bottom(size_y);
        for (auto &cell : top_and_bottom)
        {
            cell = wall;
        }

        std::vector<Tile> inside(size_y);
        inside[0]          = wall;
        inside[size_y - 1] = wall;
        for (uint32_t i = 1; i < size_y - 1; i++)
        {
            inside[i] = floor;
        }
        GameMap debug_map(size_x);
        debug_map[0]          = top_and_bottom;
        debug_map[size_x - 1] = top_and_bottom;
        for (uint32_t i = 1; i < size_x - 1; i++)
        {
            debug_map[i] = inside;
        }
        debug_map[49][24].type |= TileType::HAS_STAIRS;
        debug_map[49][24].appearance = TileAppearance::STAIRS;

        return debug_map;
    }
};

#endif /*DEBUGMAPGENERATOR_H*/
