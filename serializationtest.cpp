#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include <cstdint>
#include <iostream>
#include <memory>

/*
 * Component test status:
 * AI
 * AMULETSLOT
 * ARMOR
 * ARMORSLOT
 * BUFF
 * COORDINATES
 * CRIT
 * DESCRIPTION
 * EFFECT
 * EXPERIENCE
 * HEALTH
 * HUNGER
 * INVENTORY - TESTED
 * ITEM - TESTED
 * LOS
 * NAME
 * NAVMAP
 * TILE
 * WEAPON
 * WEAPONSLOT
 */

std::unordered_map<ComponentType, Component *> e_map;

std::vector<std::vector<NavCell>> nav(G_MAP_WIDTH,
                                      std::vector<NavCell>(G_MAP_HEIGHT));

int main()
{
    EffectComponent *e1   = new EffectComponent(Effect::BLEED, 3, 3);
    EffectComponent *e2   = new EffectComponent(Effect::HEAL, 1, 1);
    EffectComponent *e3   = new EffectComponent(Effect::POISON, 4, 1);
    EffectComponent *e4   = new EffectComponent(Effect::BLIND, 1, 10);
    EffectComponent *e5   = new EffectComponent(Effect::STRENGTH, 5, 10);
    BuffComponent   *buff = new BuffComponent({e1, e2, e3, e4, e5});
    e_map[ComponentType::AI] =
        new AIComponent(AIType::AI_MONSTER_BERSERK, AIState::ATTACK, 4, 20);
    e_map[ComponentType::AMULETSLOT]  = new AmuletSlot();
    e_map[ComponentType::ARMOR]       = new ArmorComponent((uint8_t)35);
    e_map[ComponentType::ARMORSLOT]   = new ArmorSlot();
    e_map[ComponentType::BUFF]        = new BuffComponent({e1, e2, e3, e4, e5});
    e_map[ComponentType::COORDINATES] = new Coordinates(13, 37);
    e_map[ComponentType::CRIT]        = new CritComponent(33, 3.3l, buff);
    e_map[ComponentType::DESCRIPTION] = new Description("Foo desc");
    e_map[ComponentType::EFFECT] = new EffectComponent(Effect::IRONSKIN, 10, 3);
    e_map[ComponentType::EXPERIENCE] = new ExperienceComponent(7, 404);
    e_map[ComponentType::HEALTH]     = new Health(20, 3, true);
    e_map[ComponentType::HUNGER]     = new HungerComponent(23, 100);
    e_map[ComponentType::INVENTORY]  = new Inventory();
    e_map[ComponentType::ITEM] =
        new ItemComponent(ItemType::POTION, 5, 16, Rarity::EPIC);
    e_map[ComponentType::LINEOFSIGHT] = new LOSComponent(true, 32);
    e_map[ComponentType::NAME]        = new Name("Bar name");
    e_map[ComponentType::NAVMAP]      = new NavMapComponent(nav);
    e_map[ComponentType::TILE]       = new TileComponent(TileType::MAPPED, "X");
    e_map[ComponentType::WEAPON]     = new WeaponComponent(25);
    e_map[ComponentType::WEAPONSLOT] = new WeaponSlot();
    // std::cout << "HELLO!";
    // std::unique_ptr<ItemComponent> item(new ItemComponent(ItemType::ARMOR,
    // 10, 1, Rarity::LEGENDARY, true)); std::cout << item.get() << '\n';
    // std::cin >> item.get();
    // std::cout << item.get() << '\n';
    Entity    a(EntityType::CREATURE);
    Entity    b(a);
    Entity    c(a);
    Entity    d(a);
    Inventory inv;
    inv.inventory.emplace_back(&a);
    inv.inventory.emplace_back(&b);
    inv.inventory.emplace_back(&c);
    inv.inventory.emplace_back(&d);
    std::cout << &inv << '\n';
    std::cin >> &inv;
    auto x = (*System::system_messages_)[SystemType::ENTITY];
    for (auto y : x)
    {
        auto entity_list = std::any_cast<std::shared_ptr<std::list<uint32_t>>>(
            *(y.begin() + 2));
        for (auto message : *entity_list)
        {
            std::cout << message << " ";
        }
    }
    std::cout << std::endl << "\n";
    for (auto &component : e_map)
    {
        std::cout << component.second << "\n";
    }
}
