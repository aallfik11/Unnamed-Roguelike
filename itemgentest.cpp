#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include <fstream>
#include <iostream>
#include <random>

int main()
{
    std::cout << "Item Generator Test\n" << std::endl;
    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map         = *DebugMapGenerator::generate(mt, 100, 50);
    int                depth       = 0;
    auto               itemfactory = ItemFactory(map, depth);

    auto          items            = itemfactory.generateItems();
    int           item_ctr         = 0;
    std::ofstream file_wr("generated_items.txt");
    for (auto &item : items)
    {
        file_wr << "Item " << item_ctr++ << ":    " << item << std::endl;
        std::cout << "Item " << item_ctr++ << ":    " << item << std::endl;
    }
    file_wr.close();
    std::cout << "No segfault???";
    return 0;
}
