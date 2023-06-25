#ifndef MAPMANAGER_H
#define MAPMANAGER_H
#include "../observerptr.h"
#include "../tile.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <memory>
#include <random>
#include <vector>

class MapManager
{
    using GeneratorFunction = std::function<std::vector<std::vector<Tile>>(
        std::mt19937 &, uint32_t, uint32_t)>;

    GeneratorFunction                      generator_;
    std::random_device                     rng_;
    std::mt19937                           twister_engine_;
    std::vector<std::vector<Tile>>         current_map_;
    std::array<decltype(current_map_), 26> maps_;
    observer_ptr<int>                      current_depth_;

public:
    MapManager() = delete;

    MapManager(GeneratorFunction initial_generator, observer_ptr<int> depth);

    void generate();

    void setGenerator(GeneratorFunction new_generator);

    std::vector<std::vector<Tile>> &getMap();

    void saveMaps(const std::filesystem::path &save_path);
    void loadMaps(const std::filesystem::path &load_path);
};

#endif /*MAPMANAGER_H*/
