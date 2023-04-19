#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/system.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/inventorysystem.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>
#include <thread>

std::shared_ptr<Entity> attacker(new Entity(PLAYER,
                                            {new Health(50, 50),
                                             new BuffComponent(),
                                             new ArmorComponent(),
                                             new WeaponComponent(5),
                                             new CritComponent(5, 1.5),
                                             new Inventory(),
                                             new AmuletSlot()}));

std::shared_ptr<Entity> defender(new Entity(CREATURE,
                                            {new Health(50, 50),
                                             new BuffComponent(),
                                             new ArmorComponent(10),
                                             new WeaponComponent(5),
                                             new Inventory()}));

AttackSystem att;

uint64_t              iterations = 1000;
std::atomic<uint64_t> hits       = 0;
std::atomic<uint64_t> misses     = 0;

void iterate()
{
    uint64_t it = iterations / 12;
    for (uint64_t i = 0; i < it; i++)
    {
        (att.attack(attacker, defender)) ? hits++ : misses++;
    }
}

int main()
{
    EffectSystem es;

    std::shared_ptr<EffectComponent> str(new EffectComponent(STRENGTH, 4, 10));
    std::shared_ptr<EffectComponent> irnskin(
        new EffectComponent(IRONSKIN, 1, 10));
    std::shared_ptr<EffectComponent> blind(new EffectComponent(BLIND, 1, 10));
    std::shared_ptr<BuffComponent>   buffs(new BuffComponent());

    std::shared_ptr<Entity> str_ring(new Entity(
        ITEM,
        {new ItemComponent(ItemType::RING, 1, 1, RARE),
         new BuffComponent({new EffectComponent(STRENGTH | PERMANENT, 2)})}));

    std::shared_ptr<Entity> str_ring2(new Entity(
        ITEM,
        {new ItemComponent(ItemType::RING, 1, 1, RARE),
         new BuffComponent({new EffectComponent(STRENGTH | PERMANENT, 3)})}));

    // buffs->buffs[blind->effect]   = blind;
    // buffs->buffs[str->effect]     = str;
    // buffs->buffs[irnskin->effect] = irnskin;
    // es.addEffects(attacker, buffs);
    // es.addEffects(defender, buffs);
    // es.addEffects(attacker, buffs);
    InventorySystem::addToInventory(attacker, {str_ring, str_ring2});
    InventorySystem::useItem(attacker, 0);
    InventorySystem::useItem(attacker, 1);
    InventorySystem::useItem(attacker, 0);

    auto       defender_hp = defender->getComponent<Health>();
    MapManager m(DebugMapGenerator::generate);
    m.generate(100, 50);
    auto           map = m.getMap();
    PositionSystem p(*map);

    auto x = {
        std::make_any<SystemAction::POSITION>(SystemAction::POSITION::UPDATE),
        std::make_any<std::shared_ptr<Entity>>(attacker),
        std::make_any<uint16_t>(25),
        std::make_any<uint16_t>(25)};

    (*System::system_messages_)[SystemType::POSITION].emplace_back(x);
    p.readSystemMessages();
    p.updateData();

    // std::cout << defender_hp->current_health_points << '\n';

    std::cout << "put in the number of iterations: ";
    std::cin >> iterations;

    HealthSystem hpsys;

    auto y = {
        std::make_any<std::shared_ptr<Entity>>(defender),
        std::make_any<uint16_t>(23),
        std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::DAMAGE |
                                            SystemAction::HEALTH::CURRENT)};

    // (*System::system_messages_)[SystemType::POSITION].emplace_back(y);
    //     hpsys.readSystemMessages();
    //     hpsys.updateData();
    //     hpsys.clearSystemMessages();

    std::chrono::nanoseconds duration;

    if (iterations < 10)
    {
        for (uint64_t i = 0; i < iterations; i++)
        {
            (att.attack(attacker, defender)) ? hits++ : misses++;
        }
    }
    else
    {
        std::jthread t1(iterate);
        std::jthread t2(iterate);
        std::jthread t3(iterate);
        std::jthread t4(iterate);
        std::jthread t5(iterate);
        std::jthread t6(iterate);
        std::jthread t7(iterate);
        std::jthread t8(iterate);
        std::jthread t9(iterate);
        std::jthread t10(iterate);
        std::jthread t11(iterate);
        std::jthread t12(iterate);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    hpsys.readSystemMessages();
    hpsys.updateData();
    hpsys.clearSystemMessages();
    auto t2  = std::chrono::high_resolution_clock::now();

    duration = t2 - t1;

    std::cout << "\nHit percentage in " << iterations << " iterations: "
              << (double)hits * double(100) / (double)iterations << " %";
    std::cout << "\n defender HP: " << defender_hp->current_health_points;
    std::cout << "\n Time of health message cycle was: " << duration.count();
}