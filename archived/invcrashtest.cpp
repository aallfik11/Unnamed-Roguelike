
// #include "src/game/UI/gamescreen.h"
#include "src/game/UI/inventory.h"
#include "src/game/components/components_all.h"
// #include "src/game/UI/mainmenu.h"
// #include "src/game/systems/aisystem.h"
// #include "src/game/systems/attacksystem.h"
// #include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
// #include "src/game/systems/experiencesystem.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
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
    auto               map = std::vector(100, std::vector<Tile>(50));
    EntityManager      entity_manager;
    ItemFactory        it_fac(map, depth);
    // MonsterFactory     monster_fac(it_fac, map, depth);
    //
    entity_manager.createEntity(EntityType::PLAYER,
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
    // auto               player = new Entity(EntityType::PLAYER,
    //                                        {new WeaponSlot,
    //                                         new WeaponComponent(3),
    //                                         new ArmorSlot,
    //                                         new ArmorComponent,
    //                                         new AmuletSlot,
    //                                         new CritComponent,
    //                                         new ExperienceComponent,
    //                                         new BuffComponent,
    //                                         new
    //                                         TileComponent(TileAppearance::PLAYER),
    //                                         new Coordinates,
    //                                         new HungerComponent,
    //                                         new Health(10),
    //                                         new Inventory});
    InventorySystem inv_sys;
    PositionSystem  pos_sys(map);
    auto            potion = it_fac.generatePotion();
    auto            food   = it_fac.generateFoodRation();
    auto            player = entity_manager.getEntity(1);
    inv_sys.addToInventory(player, {potion, food});
    const auto& inv_size = player->getComponent<Inventory>()->inventory.size();
    auto initial_size = inv_size;
    updateSystemData(entity_manager);

    using namespace ftxui;
    InventoryUI inv_ui(inv_sys);

    int        i           = 0;
    int        loop_amount = 1000;
    auto       screen      = ScreenInteractive::FitComponent();
    std::mutex screen_lock;
    auto       renderer = Renderer(
        [&]
        {
            screen_lock.lock();
            // screen.Post(Event::Escape);
            inv_ui.render(player, screen);
            ++i;
            screen_lock.unlock();
            return vbox(
                {text(std::to_string(i) + "/" + std::to_string(loop_amount) + "inv size: " + std::to_string(inv_size)) |
                     hcenter,
                 gaugeRight(i / static_cast<float>(loop_amount)) |
                     size(ftxui::Direction::WIDTH, Constraint::EQUAL, 100) |
                     borderLight});
        });
    auto         loop = Loop(&screen, renderer);
    // std::jthread refresher(
    //     [&]
    //     {
    //         using namespace std::chrono_literals;
    //         while (i < loop_amount)
    //         {
    //             std::this_thread::sleep_for(1ms);
    //             screen_lock.lock();
    //             screen.Post(Event::Escape);
    //             screen_lock.unlock();
    //         }
    //     });
    while (i < loop_amount)
    {
        loop.RunOnce();
        if(inv_size != initial_size)
        {
            std::cout <<"INVENTORY MODIFIED >:(";
            break;
        }
    }
    screen.Exit();
    std::cout << std::endl << "\nNO CRASH :3";
}
