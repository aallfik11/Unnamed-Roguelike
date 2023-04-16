#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/inventorysystem.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

std::shared_ptr<Entity> attacker(new Entity(PLAYER,
                                            {new Health(50, 50),
                                             new BuffComponent(),
                                             new ArmorComponent(),
                                             new WeaponComponent(5),
                                             new CritComponent(5, 1.5),
                                             new Inventory()}));

std::shared_ptr<Entity> defender(new Entity(CREATURE,
                                            {new Health(50, 50),
                                             new BuffComponent(),
                                             new ArmorComponent(50),
                                             new WeaponComponent(5),
                                             new Inventory()}));

uint64_t              iterations = 1000;
std::atomic<uint64_t> hits       = 0;
std::atomic<uint64_t> misses     = 0;

void iterate()
{
    uint64_t it = iterations / 10;
    for (uint64_t i = 0; i < it; i++)
    {
        (AttackSystem::attack(attacker, defender)) ? hits++ : misses++;
    }
}

int main()
{
    EffectSystem                     es;
    
    std::shared_ptr<EffectComponent> str(new EffectComponent(STRENGTH, 4, 10));
    std::shared_ptr<EffectComponent> irnskin(
        new EffectComponent(IRONSKIN, 1, 10));
    std::shared_ptr<EffectComponent> blind(new EffectComponent(BLIND, 1, 10));
    std::shared_ptr<BuffComponent>   buffs(new BuffComponent());
    buffs->buffs[blind->effect]   = blind;
    // buffs->buffs[str->effect]     = str;
    buffs->buffs[irnskin->effect] = irnskin;
    // es.addEffects(attacker, buffs);
    es.addEffects(defender, buffs);
    es.addEffects(attacker, buffs);

    auto defender_hp = defender->getComponent<Health>();

    // std::cout << defender_hp->current_health_points << '\n';

    std::cout << "put in the number of iterations: ";
    std::cin >> iterations;

    if (iterations < 10)
    {
        for (uint64_t i = 0; i < iterations; i++)
        {
            (AttackSystem::attack(attacker, defender)) ? hits++ : misses++;
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
    }
    std::cout << "\nHit percentage in " << iterations << " iterations: "
              << (double)hits * double(100) / (double)iterations << " %";
}