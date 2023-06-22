#include "src/game/components/buffcomponent.h"
#include "src/game/components/components_all.h"
#include "src/game/entity.h"

// #include "src/game/system.h"
// #include "src/game/systems/attacksystem.h"
// #include "src/game/systems/effectsystem.h"
// #include "src/game/systems/generators/debugmapgenerator.h"
// #include "src/game/systems/healthsystem.h"
// #include "src/game/systems/inventorysystem.h"
// #include "src/game/systems/mapmanager.h"
// #include "src/game/systems/playerinputsystem.h"
// #include "src/game/systems/positionsystem.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>

int main()
{
    // std::unordered_map<Effect, std::unique_ptr<EffectComponent>> map;
    std::cout << "HELLO!" << std::endl;
    std::unique_ptr<BuffComponent> i(new BuffComponent());
    auto                           b = i->clone();
    std::unique_ptr<Entity>        a(new Entity(EntityType::PLAYER));
    a->addComponent(i.release());
    std::unique_ptr<Name> nameptr(new Name("TEST"));
    a->addComponent(nameptr.release());
    auto name = a->getComponent<Name>();
    std::cerr << name->name << std::endl;
    if (typeid(*b) == typeid(*(b.get())))
    {
        std::cerr << "SAME\n";
    }
    std::cerr << "HeLLO AGAIN";
    std::unique_ptr<EffectComponent> eff =
        std::make_unique<EffectComponent>(Effect::HEAL, 1, 1);
    std::unique_ptr<Entity> e(
        new Entity(EntityType::PLAYER, {new Inventory()}));
    // std::unique_ptr<int> a(new int(12));
}

// AttackSystem att;

// uint64_t              iterations = 1000;
// std::atomic<uint64_t> hits       = 0;
// std::atomic<uint64_t> misses     = 0;

// void iterate()
// {
//     uint64_t it = iterations / 12;
//     for (uint64_t i = 0; i < it; i++)
//     {
//         (att.attack(attacker.get(), defender.get())) ? hits++ : misses++;
//     }
// }

// int main()
// {
//     EffectSystem    es;
//     InventorySystem inv;

//     std::unique_ptr<EffectComponent> str(new EffectComponent(STRENGTH, 4,
//     10)); std::unique_ptr<EffectComponent> irnskin(
//         new EffectComponent(IRONSKIN, 1, 10));
//     std::unique_ptr<EffectComponent> blind(new EffectComponent(BLIND, 1,
//     10)); std::unique_ptr<BuffComponent2>   buffs(new BuffComponent2());

//     std::unique_ptr<Entity> str_ring(new Entity(
//         ITEM,
//         {new ItemComponent(ItemType::RING, 1, 1, RARE),
//          new BuffComponent2({new EffectComponent(STRENGTH | PERMANENT,
//          2)})}));

//     std::unique_ptr<Entity> str_ring2(new Entity(
//         ITEM,
//         {new ItemComponent(ItemType::RING, 1, 1, RARE),
//          new BuffComponent2({new EffectComponent(STRENGTH | PERMANENT,
//          3)})}));

//     // buffs->buffs[blind->effect]   = blind;
//     // buffs->buffs[str->effect]     = str;
//     // buffs->buffs[irnskin->effect] = irnskin;
//     // es.addEffects(attacker, buffs);
//     // es.addEffects(defender, buffs);
//     // es.addEffects(attacker, buffs);
//     inv.addToInventory(attacker.get(), {str_ring.get(), str_ring2.get()});
//     inv.useItem(attacker.get(), 0);
//     inv.useItem(attacker.get(), 1);
//     inv.useItem(attacker.get(), 0);

//     auto       defender_hp = defender->getComponent<Health>();
//     MapManager m(DebugMapGenerator::generate);
//     m.generate(100, 50);
//     auto           map = m.getMap();
//     PositionSystem p(*map);

//     auto x = {
//         std::make_any<SystemAction::POSITION>(SystemAction::POSITION::UPDATE),
//         std::make_any<Entity *>(attacker.get()),
//         std::make_any<uint16_t>(25),
//         std::make_any<uint16_t>(25)};

//     (*System::system_messages_)[SystemType::POSITION].emplace_back(x);
//     // System::sendSystemMessage(SystemType::POSITION, xunique_ptr
//     p.readSystemMessages();
//     p.updateData();

//     // std::cout << defender_hp->current_health_points << '\n';

//     std::cout << "put in the number of iterations: ";
//     std::cin >> iterations;

//     HealthSystem hpsys;

//     auto y = {
//         std::make_any<Entity *>(defender.get()),
//         std::make_any<uint16_t>(23),
//         std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::DAMAGE |
//                                             SystemAction::HEALTH::CURRENT)};

//     // (*System::system_messages_)[SystemType::POSITION].emplace_back(y);
//     //     hpsys.readSystemMessages();
//     //     hpsys.updateData();
//     //     hpsys.clearSystemMessages();

//     std::chrono::nanoseconds duration;

//     if (iterations < 10)
//     {
//         for (uint64_t i = 0; i < iterations; i++)
//         {
//             (att.attack(attacker.get(), defender.get())) ? hits++ : misses++;
//         }
//     }
//     else
//     {
//         std::jthread t1(iterate);
//         std::jthread t2(iterate);
//         std::jthread t3(iterate);
//         std::jthread t4(iterate);
//         std::jthread t5(iterate);
//         std::jthread t6(iterate);
//         std::jthread t7(iterate);
//         std::jthread t8(iterate);
//         std::jthread t9(iterate);
//         std::jthread t10(iterate);
//         std::jthread t11(iterate);
//         std::jthread t12(iterate);
//     }

//     auto t1 = std::chrono::high_resolution_clock::now();
//     hpsys.readSystemMessages();
//     hpsys.updateData();
//     hpsys.clearSystemMessages();
//     auto t2  = std::chrono::high_resolution_clock::now();

//     duration = t2 - t1;

//     std::cout << "\nHit percentage in " << iterations << " iterations: "
//               << (double)hits * double(100) / (double)iterations << " %";
//     std::cout << "\n defender HP: " << defender_hp->current_health_points;
//     std::cout << "\n Time of health message cycle was: " << duration.count();

//     Entity b(*attacker);
//     std::cout << "test for debug";
// }