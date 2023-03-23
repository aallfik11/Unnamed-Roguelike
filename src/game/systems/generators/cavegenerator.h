#ifndef CAVEGENERATOR_H
#define CAVEGENERATOR_H
#include "../../tile.h"
#include <vector>
#include <random>
#include <cstdint>
#include <memory>

class CaveGenerator
{
    using GameMap = std::vector<std::vector<Tile>>;
    static void randomFillMap(GameMap &map,
                                std::mt19937 &twister_engine,
                                uint32_t size_x,
                                uint32_t size_y)
    {
        std::uniform_int_distribution<uint32_t> roll_wall_chance(0, 100);
        uint32_t wall_chance = 38;
        map.erase(map.begin(), map.end());
        map.resize(size_x);
        for (auto &row : map)
        {
            row.resize(size_y);
        }

        for (uint32_t x = 0; x < size_x; x++)
        {
            for (uint32_t y = 0; y < size_y; y++)
            {
                if (x == 0 || x == size_x - 1 || y == 0 || y == size_y - 1)
                {
                    map[x][y].type = WALL;
                    continue;
                }
                if (roll_wall_chance(twister_engine) < wall_chance)
                {
                    map[x][y].type = WALL;
                }
            }
        }
    }

    static uint8_t checkNeighbors(GameMap &map,
                                    uint32_t x,
                                    uint32_t y)
    {
        uint8_t wall_counter = 0;
        for (auto column = x - 1; column <= x + 1; column++)
        {
            for (auto cell = y - 1; cell <= y + 1; cell++)
            {
                if (!(column == x && cell == y))
                {
                    if (map[column][cell].type & WALL)
                    {
                        wall_counter++;
                    }
                }
            }
        }
        return wall_counter;
    }

    static void fourFive(GameMap &map,
                          uint32_t size_x,
                          uint32_t size_y)
    {
        uint32_t max_iterations = 15;
        GameMap map_cpy(map);

        for (auto iteration = 0; iteration < max_iterations; iteration++)
        {
            for (auto x = 1; x < size_x-1; x++)
            {
                for (auto y = 1; y < size_y-1; y++)
                {
                    auto walls = checkNeighbors(map, x, y);
                    if ((map[x][y].type & WALL) && walls <= 2)
                    {
                        map_cpy[x][y].type = FLOOR | TRAVERSIBLE;
                    }

                    else if (walls >= 5)
                        map_cpy[x][y].type = WALL;
                }
            }
            map = map_cpy;
        }
    }

    static void rectify(GameMap &map)
    {
        for (auto &x : map)
        {
            for (auto &y : x)
            {
                if (y.type & CHECKED)
                {
                    continue;
                }
                else
                {
                    y.type = WALL;
                }
            }
        }
    }

    static uint32_t countWalls(GameMap &map)
    {
        uint32_t walls = 0;
        for (auto &x : map)
        {
            for (auto &y : x)
            {
                if (y.type & WALL)
                {
                    walls++;
                }
            }
        }
        return walls;
    }

    static void floodFill(GameMap &map,
                           uint32_t x,
                           uint32_t y)
    {
        if ((map[x][y].type & FLOOR) && !(map[x][y].type & CHECKED))
        {
            map[x][y].type += CHECKED;
        }
        else
            return;
        floodFill(map, x - 1, y);
        floodFill(map, x + 1, y);
        floodFill(map, x, y - 1);
        floodFill(map, x, y + 1);
    }

public:
    static std::shared_ptr<GameMap> generate(std::mt19937 &twister_engine,
                             uint32_t size_x,
                             uint32_t size_y)
    {
        std::uniform_int_distribution<uint32_t> random_x(1, size_x - 1);
        std::uniform_int_distribution<uint32_t> random_y(1, size_y - 1);
        double map_size = size_x * size_y;
        double max_wall_percentage = 0.45l;
        GameMap map;
        while (true)
        {
            randomFillMap(map,
                            twister_engine,
                            size_x,
                            size_y);

            fourFive(map, size_x, size_y);

            auto x = random_x(twister_engine);
            auto y = random_y(twister_engine);

            while (map[x][y].type & WALL)
            {
                x = random_x(twister_engine);
                y = random_y(twister_engine);
            }

            floodFill(map, x, y);
            rectify(map);
            if ((countWalls(map) / map_size) <= max_wall_percentage)
                break;
        }
        return std::shared_ptr<GameMap>(new GameMap(map));
    }
};

#endif