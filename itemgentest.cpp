#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/entitymanager.h"
#include <fstream>
#include <iostream>
#include <random>

int main()
{
    EntityManager e_manager;
    std::cout << "Item Generator Test\n" << std::endl;
    int depth = 25;
    // std::cin >> depth;

    std::random_device rd;
    
    std::mt19937       mt(rd());
    auto               map         = *DebugMapGenerator::generate(mt, 100, 50);
    auto               itemfactory = ItemFactory(map, depth);
    auto               monsterfactory = MonsterFactory(itemfactory, map, depth);

    auto          items               = itemfactory.debugGenerateItems(20);
    auto          monsters            = monsterfactory.generateMonsters();
    int           item_ctr            = 0;
    int           monster_ctr         = 0;
    std::ofstream file_wr("generated_items.txt");
    for (auto &item : items)
    {
        file_wr << "Item " << item_ctr++ << ":    " << item << std::endl;
        // std::cout << "Item " << item_ctr++ << ":    " << item << std::endl;
    }
    file_wr.close();
    file_wr.open("generated_monsters.txt");
    for (const auto &monster : monsters)
    {
        file_wr << "Monster " << monster_ctr++ << ":   " << monster
                << std::endl;
        // std::cout << monster << std::endl;
    }
    file_wr.close();
    e_manager.readSystemMessages();
    e_manager.updateData();
    System* s = &e_manager;
    std::cout << s;
    // Entity* monster = monsterfactory.generateBaseMonster();
    // monsterfactory.generateAssassin(monster);
    // std::cout << std::endl << std::endl << monster;
    std::cout << "No segfault???";
    return 0;
}
