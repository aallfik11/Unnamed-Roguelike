#include "entity.h"
#include "rarity.h"
#include "item.h"
#include <iostream>
// #include "consumable.h"

int main()
{
    // std::cout << item.get_name_with_prefix() << std::endl
    //           << item.get_description() << std::endl
    //           << item.get_symbol();
    std::list<std::unique_ptr<Item>> inv;
    Entity chest("Oak Chest", 3, 5);
    Entity dummy("Dummy", 0, 0);
    std::unique_ptr<Item> item_ptr(new Item("Test Item", "Test Description", "I", Rarity::Legendary));
    chest.add_to_inventory(std::move(item_ptr));
    for(auto &x : chest.get_inventory())
        std::cout << x.get()->get_name_with_prefix() << std::endl;
    dummy.add_to_inventory(std::move(chest.pull_item_out(0)));
    for(auto &x : dummy.get_inventory())
        std::cout << x.get()->get_name_with_prefix();
}