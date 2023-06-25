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
    static GameMap generate(std::mt19937 &, uint32_t size_x, uint32_t size_y);
};

#endif /*DEBUGMAPGENERATOR_H*/
