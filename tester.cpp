#include "src/game/entity.h"
#include "src/game/component.h"
#include "src/game/components/components_all.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <iostream>
#include <random>
#include <chrono>

int main()
{
    // std::random_device rd;
    // std::mt19937 mt(rd());

    // Entity ent;
    // std::shared_ptr<Component> comp(new TestComponent2());
    // ent.add_component(comp);
    // ent.add_component(new TestComponent());
    // ent.add_component(new Inventory());
    // std::cout << std::boolalpha;
    // auto start = std::chrono::high_resolution_clock::now();
    // ent.has_component<Inventory>();
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto time_passed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    // auto a = ent.get_component<TestComponent>();
    // a->test = "MMM PUUU";
    // std::cout << a->test << std::endl;

    // std::cout << "time for finding component is: " << time_passed << std::endl;

    // MapManager manager(CaveGenerator::generate);
    // manager.generate(200, 50);
    // auto map_ptr = manager.getMap();
    // auto map = *map_ptr;
    // for (auto y = 0; y < map[0].size(); y++)
    // {
    //     for (auto x = 0; x < map.size(); x++)
    //     {
    //         if (map[x][y].type & WALL)
    //             std::cout << '#';
    //         else
    //             std::cout << '.';
    //     }
    //     std::cout << std::endl;
    // }
    MapManager manager(DebugMapGenerator::generate);
    manager.generate(100, 50);
    PositionSystem pos_system(*(manager.getMap()));
    std::shared_ptr<Entity> player(new Entity());
    std::shared_ptr<Entity> monster(new Entity());
    std::shared_ptr<Entity> chest(new Entity());
    player->addComponent(new Name("Steve"));
    player->addComponent(new TileComponent(TileType::SPRITE, "@", ftxui::Color::Green1));
    player->addComponent(new Coordinates(1, 1));

    monster->addComponent(new TileComponent(TileType::SPRITE, "M"));
    monster->addComponent(new Coordinates(2, 2));

    chest->addComponent(new Inventory());
    chest->addComponent(new TileComponent(TileType(SPRITE | TRAVERSIBLE), " ", ftxui::Color::White, ftxui::Color::RosyBrown));
    chest->addComponent(new Coordinates(3, 3));
    pos_system.addEntity(player);
    pos_system.addEntity(monster);
    pos_system.addEntity(chest);
    std::cout << std::boolalpha << pos_system.updatePosition(player->getId(), 3, 3) << std::endl
              << pos_system.updatePosition(monster->getId(), 3, 3) << std::endl
              << std::endl
              << pos_system.coords_with_entities_.size() << std::endl;
    std::cout << "All entity Ids: ";
    auto ids = pos_system.getAllEntityIds();
    for (auto id : ids)
    {
        std::cout << id << ' ';
    }
    std::cout << std::endl
              << "Entity ids stored at coordinates x = 3, y = 3: ";
    ids = pos_system.getEntityIds(3, 3);
    for (auto id : ids)
    {
        std::cout << id << ' ';
    }
    auto player_coord_ptr = pos_system.getEntityCoordinates(player->getId());
    std::cout << std::endl
              << "player coordinates are: " << player_coord_ptr->x << ' ' << player_coord_ptr->y;
    pos_system.deleteEntity(player->getId());
    std::cout << std::endl
              << std::endl
              << "After player's deletion, entities at 3,3: ";
    ids = pos_system.getEntityIds(3, 3);
    for (auto id : ids)
    {
        std::cout << id << ' ';
    }

    std::cout << std::endl;
    std::cout << "All ids after player's deletion: ";
    ids = pos_system.getAllEntityIds();
    for (auto id : ids)
    {
        std::cout << id << ' ';
    }
    std::cout << std::endl
              << pos_system.updatePosition(monster->getId(), 3, 3) << std::endl;


    for (auto x : pos_system.coords_with_entities_)
    {
        std::cout << x.first.first << ' ' << x.first.second << "id: " << x.second << std::endl;
    }
    return 0;
}
