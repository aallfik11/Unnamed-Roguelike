#include "src/game/component.h"
#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <iostream>
#include <random>

int main()
{

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
    std::cout << std::boolalpha << pos_system.updatePosition(player, 3, 3) << std::endl
              << pos_system.updatePosition(monster, 3, 3) << std::endl
              << pos_system.updatePosition(player, 5, 5) << std::endl
              << pos_system.updatePosition(monster, 4, 4) << std::endl;

    return 0;
}
