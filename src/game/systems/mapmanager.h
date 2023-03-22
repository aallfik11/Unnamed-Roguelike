#ifndef MAPMANAGER_H
#define MAPMANAGER_H
#include "../tile.h"
#include <functional>
#include <cstdint>
#include <random>
#include <memory>
#include <vector>

class MapManager
{
    using MapPointer = std::shared_ptr<std::vector<std::vector<Tile>>>;
    using GeneratorFunction = std::function<MapPointer(std::mt19937 &, uint32_t, uint32_t)>;

    GeneratorFunction _generator;
    std::random_device rng;
    std::mt19937 twister_engine_;
    MapPointer map_;

public:
    MapManager(GeneratorFunction initial_generator = 0) : _generator{initial_generator}
    {
        twister_engine_ = std::mt19937(rng());
    }

    void generate(uint32_t size_x, uint32_t size_y)
    {
        map = _generator(twister_engine_, size_x, size_y);
    }

    void setGenerator(GeneratorFunction new_generator)
    {
        _generator = new_generator;
    }

    MapPointer getMap()
    {
        return map;
    }
};

#endif /*MAPMANAGER_H*/
