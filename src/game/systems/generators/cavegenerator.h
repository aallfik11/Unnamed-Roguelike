#ifndef CAVEGENERATOR_H
#define CAVEGENERATOR_H
#include "../../tile.h"
#include <cstdint>
#include <random>
#include <vector>

class CaveGenerator
{
    using GameMap = std::vector<std::vector<Tile>>;
    static void randomFillMap(GameMap      &map,
                              std::mt19937 &twister_engine,
                              uint32_t      size_x,
                              uint32_t      size_y);

    static uint8_t checkNeighbors(GameMap &map, uint32_t x, uint32_t y);

    static void fourFive(GameMap &map, uint32_t size_x, uint32_t size_y);

    static void rectify(GameMap &map);

    static uint32_t countWalls(GameMap &map);

    static void floodFill(GameMap &map, uint32_t x, uint32_t y);

public:
    static GameMap
    generate(std::mt19937 &twister_engine, uint32_t size_x, uint32_t size_y);
};

#endif
