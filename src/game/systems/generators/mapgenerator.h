#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H
#include "../../tile.h"
#include <functional>
#include <cstdint>
#include <random>
#include <vector>

class MapGenerator
{
    using GeneratorFunction = std::function<std::vector<std::vector<Tile>> &(uint32_t, uint32_t, uint32_t)>;
    GeneratorFunction _generator;
    std::random_device rng;
    std::mt19937 _twister_engine;

public:
    MapGenerator(GeneratorFunction initial_generator = 0) : _generator{initial_generator}
    {
        _twister_engine = std::mt19937(rng());
    }

    std::vector<std::vector<Tile>> &generate(uint32_t size_x, uint32_t size_y)
    {
        return _generator(rng(), size_x, size_y);
    }

    void set_generator(GeneratorFunction new_generator)
    {
        _generator = new_generator;
    }
};

#endif /*MAPGENERATOR*/