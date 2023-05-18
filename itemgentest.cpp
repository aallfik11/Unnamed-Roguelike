#include "src/game/systems/entitymanager.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

namespace fs = std::filesystem;

int main()
{
    auto entities_path  = fs::current_path() / "entities";

    auto e1_path        = entities_path / fs::path("entities1.txt");
    auto e2_path        = entities_path / fs::path("entities2.txt");

    auto generated_path = fs::current_path() / "generated";
    auto monster_path   = generated_path / fs::path("generated_monsters.txt");
    auto item_path      = generated_path / fs::path("generated_items.txt");

    fs::create_directory(entities_path);
    fs::create_directory(generated_path);

    EntityManager e_manager;
    std::cout << "Item Generator Test\n" << std::endl;
    int depth = 24;
    // std::cin >> depth;

    std::random_device rd;

    std::mt19937 mt(rd());
    auto         map            = *DebugMapGenerator::generate(mt, 100, 50);
    auto         itemfactory    = ItemFactory(map, depth);
    auto         monsterfactory = MonsterFactory(itemfactory, map, depth);

    auto          items         = itemfactory.debugGenerateItems(20);
    auto          monsters      = monsterfactory.generateMonsters();
    int           item_ctr      = 0;
    int           monster_ctr   = 0;
    std::ofstream file_wr(item_path);
    for (auto &item : items)
    {
        file_wr << "Item " << item_ctr++ << ":    " << item << std::endl;
        // std::cout << "Item " << item_ctr++ << ":    " << item << std::endl;
    }
    file_wr.close();
    file_wr.open(monster_path);
    for (const auto &monster : monsters)
    {
        file_wr << "Monster " << monster_ctr++ << ":   " << monster
                << std::endl;
        // std::cout << monster << std::endl;
    }
    file_wr.close();
    e_manager.readSystemMessages();
    e_manager.updateData();
    e_manager.clearSystemMessages();
    System *s = &e_manager;
    // std::cout << s;
    // file_wr.open("C:\\Users\\mniec\\OneDrive\\Pulpit\\RoguelikeProjekt\\Unnamed-Roguelike\\build\\entities.txt");
    file_wr.open(e1_path);
    file_wr << s;
    file_wr.close();
    // Entity* monster = monsterfactory.generateBaseMonster();
    // monsterfactory.generateAssassin(monster);
    // std::cout << std::endl << std::endl << monster;
    EntityManager e_manager2;
    System       *e2 = &e_manager2;
    std::ifstream ifstr(e1_path);
    int           temp{};
    ifstr >> temp;
    ifstr >> e2;
    ifstr.close();

    e2->readSystemMessages();
    e2->updateData();
    e2->clearSystemMessages();

    file_wr.open(e2_path);
    file_wr << e2;
    file_wr.close();
    if (e_manager == e_manager2)
    {
        std::cout << "Same!";
    }
    std::cout << "No segfault???";
    return 0;
}
