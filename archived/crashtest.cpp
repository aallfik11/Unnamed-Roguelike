#include "src/game/UI/gamescreen.h"
#include "src/game/components/components_all.h"
// #include "src/game/UI/inventory.h"
// #include "src/game/UI/mainmenu.h"
// #include "src/game/systems/aisystem.h"
// #include "src/game/systems/attacksystem.h"
// #include "src/game/systems/effectsystem.h"
// #include "src/game/systems/entitymanager.h"
// #include "src/game/systems/experiencesystem.h"
// #include "src/game/systems/factories/itemfactory.h"
// #include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
// #include "src/game/systems/healthsystem.h"
// #include "src/game/systems/inventorysystem.h"
// #include "src/game/systems/lineofsightsystem.h"
// #include "src/game/systems/mapmanager.h"
// #include "src/game/systems/playerinputsystem.h"
#include "src/game/systems/positionsystem.h"
#include <cstdint>
#include <ftxui/component/loop.hpp>
#include <random>

void updateSystemData(System &system)
{
    system.readSystemMessages();
    system.updateData();
    system.clearSystemMessages();
}

void updateSystemData(std::initializer_list<System *> systems)
{
    for (auto &system : systems)
    {
        updateSystemData(*system);
    }
}

int main()
{
    int                depth = 0;
    std::random_device rd;
    std::mt19937       mt(rd());
    // auto               map    = *CaveGenerator::generate(mt, 100, 50);
    auto map = std::vector(100, std::vector<Tile>(50));
    // EntityManager      entity_manager;
    // ItemFactory        it_fac(map, depth);
    // MonsterFactory     monster_fac(it_fac, map, depth);
    //
    // entity_manager.createEntity(EntityType::PLAYER,
    //                             {new WeaponSlot,
    //                              new WeaponComponent(3),
    //                              new ArmorSlot,
    //                              new ArmorComponent,
    //                              new AmuletSlot,
    //                              new CritComponent,
    //                              new ExperienceComponent,
    //                              new BuffComponent,
    //                              new TileComponent(TileAppearance::PLAYER),
    //                              new Coordinates,
    //                              new HungerComponent,
    //                              new Health(10),
    //                              new Inventory});
    auto               player = new Entity(EntityType::PLAYER,
                                           {new WeaponSlot,
                                            new WeaponComponent(3),
                                            new ArmorSlot,
                                            new ArmorComponent,
                                            new AmuletSlot,
                                            new CritComponent,
                                            new ExperienceComponent,
                                            new BuffComponent,
                                            new TileComponent(TileAppearance::PLAYER),
                                            new Coordinates,
                                            new HungerComponent,
                                            new Health(10),
                                            new Inventory});
    //
    // InventorySystem inv_sys;
    // HealthSystem    health_sys;
    // AttackSystem    attack_sys;
    PositionSystem     pos_sys(map);
    // auto            goblin = monster_fac.generateBaseMonster();
    // monster_fac.generateGoblin(goblin);
    // monster_fac.placeMonster(goblin);
    // goblin->getComponent<ArmorSlot>()->armor_item = nullptr;
    // updateSystemData(entity_manager);
    // auto player = entity_manager.getEntity(1);

    // while (goblin->getComponent<Health>()->alive == true)
    // {
    //     attack_sys.attack(player, goblin);
    //     updateSystemData(health_sys);
    // }
    // int  *ptr    = &depth;
    // auto &ref    = ptr;
    // auto  any    = std::make_any<int *>(ref);
    // auto  casted = std::any_cast<int *>(any);
    // std::cout << *casted << "\n";

    // updateSystemData(inv_sys);
    GameScreen gs(&map, player, pos_sys);

    using namespace ftxui;
    // for (int i = 0; i < 10000; ++i)
    // {
    //     std::cout << i << std::flush;
    //     gs.render();
    //     std::cout << " finished" << std::endl;
    // }
    using namespace ftxui;
    int  i           = 0;
    int  loop_amount = 10000;
    auto renderer    = Renderer(
        [&]
        {
            gs.getMapBox();
            ++i;
            return vbox(
                {text(std::to_string(i) + "/" + std::to_string(loop_amount)) |
                     hcenter,
                 gaugeRight(i / static_cast<float>(loop_amount)) |
                     size(ftxui::Direction::WIDTH, Constraint::EQUAL, 100) |
                     borderLight});
        });
    auto         screen = ScreenInteractive::FitComponent();
    auto         loop   = Loop(&screen, renderer);
    std::jthread refresher(
        [&]
        {
            using namespace std::chrono_literals;
            while (i < loop_amount)
            {
                std::this_thread::sleep_for(1ms);
                screen.Post(Event::Custom);
            }
        });
    while (i < loop_amount)
    {
        loop.RunOnce();
    }
    screen.Exit();
    // updateSystemData(pos_sys);
    std::cout << std::endl << "\nNO CRASH :3";
}
