#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/systems/inventorysystem.h"
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>

int main()
{
    std::shared_ptr<Entity> player(new Entity(PLAYER,
                                              {
                                                  new Inventory(),
                                                  new ArmorSlot(),
                                                  new WeaponSlot(),
                                                  new AmuletSlot(),
                                                  new BuffComponent(),
                                                  new Health(20, 20),
                                              }));

    std::shared_ptr<Entity> sword(new Entity(
        ITEM,
        {
            new Name("Sword of foo"),
            new Description("This sword does foo"),
            new WeaponComponent(10),
            new ItemComponent(
                ItemType::WEAPON, /*stack*/ 1, /*max_stack*/ 1, EPIC),

        }));

    std::shared_ptr<Entity> armor(new Entity(
        ITEM,
        {
            new Name("Armor of bar"),
            new Description("This armor used to belong to bar"),
            new ArmorComponent(40),
            new ItemComponent(
                ItemType::ARMOR, /*stack*/ 1, /*max_stack*/ 1, LEGENDARY),
        }));

    InventorySystem invsys;
    invsys.addToInventory(player->getComponent<Inventory>()->inventory,
                          {sword, armor});

    invsys.useItem(player, 1);
    // invsys.useItem(player, 1);
    invsys.dropFromInventory(player, 1);

    for (auto item : player->getComponent<Inventory>()->inventory)
    {
        auto item_component   = item->getComponent<ItemComponent>();
        auto item_name        = item->getComponent<Name>();
        auto item_description = item->getComponent<Description>();

        std::cout << item_name->name << std::endl
                  << item_description->description << std::endl
                  << item_component->stack << '/' << item_component->max_stack
                  << std::endl
                  << "Rarity: " << static_cast<int>(item_component->rarity)
                  << std::endl
                  << "Equipped: " << std::boolalpha << item_component->equipped
                  << std::endl
                  << std::endl;
    }

    std::cout << "I didn't segfault!";
}