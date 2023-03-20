#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H
#include "../../tile.h"
#include <functional>
#include <cstdint>
#include <random>
#include <memory>
#include <vector>

class MapGenerator
{
    using MapPointer = std::shared_ptr<std::vector<std::vector<Tile>>>;
    using GeneratorFunction = std::function<MapPointer(std::mt19937 &, uint32_t, uint32_t)>;

    GeneratorFunction _generator;
    std::random_device rng;
    std::mt19937 _twister_engine;

public:
    MapGenerator(GeneratorFunction initial_generator = 0) : _generator{initial_generator}
    {
        _twister_engine = std::mt19937(rng());
    }

    MapPointer generate(uint32_t size_x, uint32_t size_y)
    {
        return _generator(_twister_engine, size_x, size_y);
    }

    void set_generator(GeneratorFunction new_generator)
    {
        _generator = new_generator;
    }
};

#endif /*MAPGENERATOR*/