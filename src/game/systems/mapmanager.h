#ifndef MAPMANAGER_H
#define MAPMANAGER_H
#include "../globals.h"
#include "../observerptr.h"
#include "../tile.h"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <random>
#include <ranges>
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

    MapManager(GeneratorFunction initial_generator, observer_ptr<int> depth)
        : generator_{initial_generator}, current_depth_{depth}
    {
        twister_engine_ = std::mt19937(rng_());
    }

    void generate()
    {
        for (auto &map : maps_)
        {
            map = generator_(twister_engine_, G_MAP_WIDTH, G_MAP_HEIGHT);
        }
        for (auto &row : maps_[25])
        {
            for (auto &cell :
                 row | std::views::filter(
                           [](Tile &t)
                           {
                               if (t.appearance == TileAppearance::STAIRS)
                               {
                                   return true;
                               }
                               return false;
                           }))
            {
                cell.type       &= ~(TileType::HAS_STAIRS);
                cell.appearance  = TileAppearance::FLOOR;
            }
        }
    }

    void setGenerator(GeneratorFunction new_generator)
    {
        generator_ = new_generator;
    }

    std::vector<std::vector<Tile>> &getMap()
    {
        return maps_.at(*current_depth_);
    }

    void saveMaps(const std::filesystem::path &save_path)
    {
        using namespace std::filesystem;
        std::string   savefile_name = "map_";
        int           i             = 0;
        std::ofstream saver(save_path / path("depth"));
        saver << *current_depth_;
        saver.close();
        for (auto &map : maps_)
        {
            saver.open(save_path / path(savefile_name + std::to_string(i)));
            for (auto &col : map)
            {
                for (auto &cell : col)
                {
                    saver << cell << ' ';
                }
                saver << std::endl;
            }
            saver.close();
            ++i;
        }
    }
    void loadMaps(const std::filesystem::path &load_path)
    {

        using namespace std::filesystem;
        std::string   savefile_name = "map_";
        int           i             = 0;
        std::ifstream loader(load_path / path("depth"));
        loader >> *current_depth_;
        loader.close();
        for (auto &map : maps_)
        {
            loader.open(load_path / path(savefile_name + std::to_string(i)));
            std::vector<std::vector<Tile>> temp_map(
                G_MAP_WIDTH, std::vector<Tile>(G_MAP_HEIGHT));
            for (auto &col : temp_map)
            {
                for (auto &cell : col)
                {
                    loader >> cell;
                }
            }
            loader.close();
            map = std::move(temp_map);
            ++i;
        }
    }
};

#endif /*MAPMANAGER_H*/
