#include "entity.h"
#include "item.h"
#include "rarity.h"
#include <iostream>

// #include "consumable.h"

int main()
{
    // std::cout << item.get_name_with_prefix() << std::endl
    //           << item.get_description() << std::endl
    //           << item.get_symbol();
    std::list<std::unique_ptr<Item>> inv;
    Entity                chest("Oak Chest", "H", ftxui::Color::White, 3, 5);
    Entity                dummy("Dummy", "D", ftxui::Color::White, 0, 0);
    Entity                dummy2(dummy);
    std::unique_ptr<Item> item_ptr(
        new Item("Test Item", "Test Description", "I", Rarity::Legendary));
    chest.add_to_inventory(item_ptr);
    for (auto &x : chest.get_inventory())
        std::cout << x.get()->get_name_with_prefix() << std::endl;
    dummy.add_to_inventory(chest.pull_item_out());
    for (auto &x : dummy.get_inventory())
        std::cout << x.get()->get_name_with_prefix() << std::endl;
    dummy.transfer_inventory(&dummy2);
    for (auto &x : dummy2.get_inventory())
        std::cout << x.get()->get_name_with_prefix();
}