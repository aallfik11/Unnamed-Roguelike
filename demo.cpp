#include "src/game/UI/gamescreen.h"
#include "src/game/UI/inventory.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/aisystem.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
#include "src/game/systems/experiencesystem.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/inventorysystem.h"
#include "src/game/systems/lineofsightsystem.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/playerinputsystem.h"
#include "src/game/systems/positionsystem.h"
#include <cstdint>
#include <random>

int main()
{
    MainMenu           main_menu;
    LaunchOptions      option{};
    std::random_device rd;
    std::mt19937       mt(rd());
    main_menu.renderMainMenu(option);
    if(option == LaunchOptions::EXIT)
    {
        return 0;
    }
    auto          map = *CaveGenerator::generate(mt, 100, 50);
    EntityManager entity_manager;

    entity_manager.createEntity(EntityType::PLAYER,
                                {new WeaponSlot,
                                 new WeaponComponent(1000),
                                 new ArmorSlot,
                                 new ArmorComponent(100),
                                 new AmuletSlot,
                                 new CritComponent,
                                 new ExperienceComponent,
                                 new BuffComponent,
                                 new TileComponent(TileAppearance::PLAYER),
                                 new Coordinates,
                                 new HungerComponent,
                                 new Health(10),
                                 new Inventory});

    int            depth = 0;
    ItemFactory    it_fac(map, depth);
    MonsterFactory monster_fac(it_fac, map, depth);

    it_fac.generateItems();
    monster_fac.generateMonsters();
    std::list<Entity*> food;
    for(int i = 0; i < 10; ++i)
    {
        food.emplace_back(it_fac.generateFoodRation());
    }
    
    // auto monster = monster_fac.generateBaseMonster();
    // monster_fac.generateBat(monster);
    // monster_fac.placeMonster(monster);
    // auto message = {
    //     std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
    //     std::make_any<Entity *>(monster)};
    // System::sendSystemMessage(SystemType::ENTITY, message);
    // System::sendSystemMessage(
    //     SystemType::AI,
    //     {std::make_any<SystemAction::AI>(SystemAction::AI::ADD),
    //      std::make_any<Entity *>(monster)});
    //
    // System::sendSystemMessage(SystemType::LINE_OF_SIGHT,
    //                           {std::make_any<SystemAction::LINE_OF_SIGHT>(
    //                                SystemAction::LINE_OF_SIGHT::ADD),
    //                            std::make_any<Entity *>(monster)});
    // System::sendSystemMessage(
    //     SystemType::EFFECT,
    //     {std::make_any<SystemAction::EFFECT>(SystemAction::EFFECT::ADD_ENTITY),
    //      std::make_any<Entity *>(monster)});

    NavMapManager nav_map_manager(
        map, entity_manager.getEntity(1)->getComponent<Coordinates>());
    uint16_t x{}, y{};

    std::uniform_int_distribution<uint16_t> rand_x(1, 98);
    std::uniform_int_distribution<uint16_t> rand_y(1, 48);
    x = rand_x(mt);
    y = rand_y(mt);

    PositionSystem pos_system(map);
    entity_manager.readSystemMessages();
    entity_manager.updateData();
    entity_manager.clearSystemMessages();
    pos_system.readSystemMessages();
    pos_system.updateData();
    pos_system.clearSystemMessages();

    while ((map[x][y].type & (TileType::WALL | TileType::HAS_ITEM |
                              TileType::HAS_CREATURE | TileType::HAS_STAIRS)) !=
           TileType::NONE)
    {
        x = rand_x(mt);
        y = rand_y(mt);
    }
    pos_system.updatePosition(entity_manager.getEntity(1), x, y);
    nav_map_manager.calculatePlayerNavMap();
    AISystem ai_system(map, nav_map_manager, entity_manager.getEntity(1));
    auto    &player_hp = entity_manager.getEntity(1)
                          ->getComponent<Health>()
                          ->current_health_points;

    GameScreen game_screen(&map, entity_manager.getEntity(1), pos_system);

    InventorySystem inv_sys;
    InventoryUI     inv_ui(inv_sys);

    inv_sys.addToInventory(entity_manager.getEntity(1), food);

    using namespace ftxui;

    auto scr      = ScreenInteractive::Fullscreen();

    auto renderer = Renderer([&] { return game_screen.render(); });

    bool                            exit           = false;
    bool                            update_systems = false;
    std::list<observer_ptr<System>> systems;

    PlayerControlSystem player_sys(entity_manager.getEntity(1), pos_system);
    EffectSystem        effect_sys;
    AttackSystem        attack_sys;
    HealthSystem        health_sys;
    LOS_System          los_system(map, entity_manager.getEntity(1));
    ExperienceSystem    exp_sys(entity_manager.getEntity(1));

    systems.emplace_back(&entity_manager);
    systems.emplace_back(&player_sys);
    systems.emplace_back(&los_system);
    systems.emplace_back(&ai_system);
    systems.emplace_back(&attack_sys);
    systems.emplace_back(&effect_sys);
    systems.emplace_back(&health_sys);
    systems.emplace_back(&exp_sys);
    systems.emplace_back(&inv_sys);
    systems.emplace_back(&pos_system);

    auto input_handler = CatchEvent(
        renderer,
        [&](Event event)
        {
            if (event.is_mouse())
            {
                update_systems = false;
                // return false;
            }
            if (event == Event::Escape)
            {
                // scr.Exit();
                update_systems = false;
                exit           = true;
                return true;
            }
            if (event == Event::Character("i") ||
                event == Event::Character("I"))
            {
                inv_ui.render(entity_manager.getEntity(1), scr);
                update_systems = false;
                return false;
            }
            if (event == Event::ArrowUp)
            {
                System::sendSystemMessage(
                    SystemType::PLAYER,
                    {std::make_any<SystemAction::PLAYER>(
                         SystemAction::PLAYER::MOVE),
                     std::make_any<::Direction>(::Direction::UP)});
                update_systems = true;
            }
            if (event == Event::ArrowDown)
            {
                System::sendSystemMessage(
                    SystemType::PLAYER,
                    {std::make_any<SystemAction::PLAYER>(
                         SystemAction::PLAYER::MOVE),
                     std::make_any<::Direction>(::Direction::DOWN)});
                update_systems = true;
            }
            if (event == Event::ArrowLeft)
            {
                System::sendSystemMessage(
                    SystemType::PLAYER,
                    {std::make_any<SystemAction::PLAYER>(
                         SystemAction::PLAYER::MOVE),
                     std::make_any<::Direction>(::Direction::LEFT)});
                update_systems = true;
            }
            if (event == Event::ArrowRight)
            {
                System::sendSystemMessage(
                    SystemType::PLAYER,
                    {std::make_any<SystemAction::PLAYER>(
                         SystemAction::PLAYER::MOVE),
                     std::make_any<::Direction>(::Direction::RIGHT)});
                update_systems = true;
            }
            if (update_systems)
            {

                for (auto &system : systems)
                {
                    system->readSystemMessages();
                    system->updateData();
                    system->clearSystemMessages();
                }
                update_systems = false;
            }
            return false;
        });

    auto loop = Loop(&scr, input_handler);

    while (player_hp > 0 && exit == false)
    {
        loop.RunOnce();
        // loop.RunOnce();
        // if (update_systems)
        //     for (auto &system : systems)
        //     {
        //         system->readSystemMessages();
        //         system->updateData();
        //         system->clearSystemMessages();
        //     }
    }
}
