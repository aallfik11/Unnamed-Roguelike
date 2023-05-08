#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include <fstream>
#include <iostream>
#include <random>

int main()
{
    std::cout << "Item Generator Test\n" << std::endl;
    int depth = 0;
    std::cin >> depth;

    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map         = *DebugMapGenerator::generate(mt, 100, 50);
    auto               itemfactory = ItemFactory(map, depth);

    auto          items            = itemfactory.debugGenerateItems(20);
    int           item_ctr         = 0;
    std::ofstream file_wr("generated_items.txt");
    for (auto &item : items)
    {
        file_wr << "Item " << item_ctr++ << ":    " << item << std::endl;
        // std::cout << "Item " << item_ctr++ << ":    " << item << std::endl;
    }
    file_wr.close();
    std::cout << "No segfault???";
    Description desc("TEST DESCRIPTION UWU XD");
    std::cout << std::endl << &desc << "is there any whitespace?" << std::endl;
    Description desc2;
    std::cin >> &desc2;
    std::cout << std::endl << &desc2 << "is there any whitespace?";
    return 0;
}
