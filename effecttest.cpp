#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/inventorysystem.h"
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <memory>

int main()
{
    std::shared_ptr<Entity> player(new Entity(PLAYER,
                                              {
                                                  new Inventory(),
                                                  new ArmorSlot(),
                                                  new WeaponSlot(),
                                                  new AmuletSlot(),
                                                  new BuffComponent(),
                                                  new Health(20, 5),
                                              }));

    std::shared_ptr<Entity> test_potion(
        new Entity(ITEM,
                   {new Name("Test potion"),
                    new Description("Test potion desc"),
                    new BuffComponent(
                        {new EffectComponent(Effect::HEAL, /*strength */ 3)}),
                    new ItemComponent(ItemType::POTION, 1, 16, Rarity::UNCOMMON)

                   }));

    std::shared_ptr<Entity> test_potion2(
        new Entity(ITEM,
                   {new Name("Test potion2"),
                    new Description("Test potion desc2"),
                    new BuffComponent(
                        {new EffectComponent(Effect::POISON, /*strength */ 3, 2)}),
                    new ItemComponent(ItemType::POTION, 1, 16, Rarity::UNCOMMON)

                   }));

    InventorySystem::addToInventory(
        player->getComponent<Inventory>()->inventory,
        {test_potion, test_potion2, test_potion});

    auto hp = player->getComponent<Health>()->current_health_points;
    InventorySystem::useItem(player, 0);
    EffectSystem effectsys;
    effectsys.addEntity(player);
    effectsys.updateEffects();
    InventorySystem::useItem(player, 1);
    effectsys.updateEffects();
    effectsys.updateEffects();

    auto hp_2 = player->getComponent<Health>()->current_health_points;
    std::cout << hp << " -> " << hp_2;
}