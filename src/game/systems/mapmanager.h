#ifndef MAPMANAGER_H
#define MAPMANAGER_H
#include "../tile.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <random>
#include <vector>

class MapManager
{
    using MapPointer = std::shared_ptr<std::vector<std::vector<Tile>>>;
    using GeneratorFunction =
        std::function<MapPointer(std::mt19937 &, uint32_t, uint32_t)>;

    GeneratorFunction  generator_;
    std::random_device rng_;
    std::mt19937       twister_engine_;
    MapPointer         map_;

public:
    MapManager(GeneratorFunction initial_generator = 0)
        : generator_{initial_generator}
    {
        twister_engine_ = std::mt19937(rng_());
    }

    void generate(uint32_t size_x, uint32_t size_y)
    {
        map_ = generator_(twister_engine_, size_x, size_y);
    }

    void setGenerator(GeneratorFunction new_generator)
    {
        generator_ = new_generator;
    }

    MapPointer getMap() { return map_; }
};

#endif /*MAPMANAGER_H*/
