#include "src/game/UI/gamescreen.h"
#include "src/game/UI/highscoresaver.h"
#include "src/game/UI/inventory.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/UI/savescreen.h"
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
#include "src/game/systems/playersystem.h"
#include "src/game/systems/positionsystem.h"
#include "src/permissive_fov/permissive-fov-cpp.h"
#include <cstdint>
#include <filesystem>
#include <random>

uint64_t calculateScore(uint16_t level, uint32_t experience = 0)
{
    if (level == 1)
    {
        return experience;
    }
    if (level == 2)
    {
        return level * 25;
    }
    return calculateScore(level - 1) + level * 25 + experience;
}

int main()
{
    auto               scr = ftxui::ScreenInteractive::Fullscreen();
    MainMenu           main_menu(scr);
    SaveScreen         save_screen(scr);
    HighScoreSaver     highscore_saver(scr);
    LaunchOptions      option{};
    bool               next_level = false;
    bool               quit       = false;
    std::string        savefile_path{};
    std::random_device rd;
    std::mt19937       mt(rd());
    std::filesystem::create_directory(std::filesystem::current_path() /
                                      std::filesystem::path("saves"));
    auto mask_folder = std::filesystem::current_path() / "masks";
    if (std::filesystem::exists(mask_folder) == false)
    {
        std::filesystem::create_directory(mask_folder);
    }
    auto mask =
        permissive::maskT((mask_folder / "circlemask.mask").string().c_str());
    int              depth{};
    MapManager       map_manager(CaveGenerator::generate, &depth);
    EntityManager    entity_manager;
    NavMapManager    nav_map_manager;
    PlayerSystem     player_control_system(next_level);
    LOS_System       los_system;
    AISystem         ai_system;
    AttackSystem     attack_system;
    EffectSystem     effect_system;
    HealthSystem     health_system;
    ExperienceSystem experience_system;
    InventorySystem  inventory_system;
    PositionSystem   position_system;
    ItemFactory      it_fac(depth);
    MonsterFactory   monster_fac(it_fac, depth);

    std::list<std::reference_wrapper<System>> systems;
    systems.emplace_back(entity_manager);
    systems.emplace_back(nav_map_manager);
    systems.emplace_back(player_control_system);
    systems.emplace_back(los_system);
    systems.emplace_back(ai_system);
    systems.emplace_back(attack_system);
    systems.emplace_back(effect_system);
    systems.emplace_back(health_system);
    systems.emplace_back(experience_system);
    systems.emplace_back(inventory_system);
    systems.emplace_back(position_system);

    auto save = [&](const std::string &save_name = "autosave")
    {
        using namespace std::filesystem;

        auto save_path = path(std::filesystem::current_path() / path("saves") /
                              path(save_name));
        auto systems_path = save_path / "systems.txt";
        auto map_path     = save_path / "maps";
        create_directories(save_path);
        create_directories(map_path);
        std::ofstream saver(systems_path);
        for (auto &system : systems)
        {
            saver << system << '\n';
        }
        saver.close();
        map_manager.saveMaps(map_path);
    };

    auto updateSystems = [&]
    {
        for (auto &system : systems)
        {
            system.get().readSystemMessages();
            system.get().updateData();
            system.get().clearSystemMessages();
        }
    };

    auto resetSystems = [&](bool hard = false)
    {
        if (hard)
        {
            entity_manager.hardReset();
        }
        for (auto &system : systems)
        {
            system.get().resetSystem();
        }
    };

    auto inputHighscore = [&]
    {
        auto player_xp =
            entity_manager.getEntity(1)->getComponent<ExperienceComponent>();
        auto highscore =
            calculateScore(player_xp->level, player_xp->current_experience);
        highscore_saver.render(highscore);
    };

    while (true)
    {

        main_menu.renderMainMenu(option, savefile_path);
        switch (option)
        {
        case LaunchOptions::LOAD:
            [[fallthrough]];
        case LaunchOptions::LAUNCH:
            resetSystems(true);
            depth      = 0;
            quit       = false;
            next_level = false;
            break;
        case LaunchOptions::EXIT:
            return 0;
            break;
        default:
            return -1;
        }

        using namespace ftxui;
        if (option == LaunchOptions::LOAD && savefile_path.empty() == false)
        {
            using namespace std::filesystem;
            auto save_path    = path(std::filesystem::current_path() /
                                  path("saves") / path(savefile_path));
            auto systems_path = save_path / "systems.txt";
            auto map_path     = save_path / "maps";
            bool save_path_not_exists =
                !(exists(save_path) && (save_path.empty() == false));
            bool map_not_exists =
                !(exists(map_path) &&
                  (std::distance(directory_iterator(map_path),
                                 directory_iterator{}) == 27));
            bool systems_not_exist =
                !(exists(systems_path) && (systems_path.empty() == false));

            if (save_path_not_exists || map_not_exists || systems_not_exist)
            {
                using namespace ftxui;
                auto renderer = Renderer(
                    []
                    {
                        return text(
                            "Invalid savefile. Press ENTER to go "
                            "back to the main menu or ESCAPE to quit the game");
                    });
                auto exit        = false;
                auto key_handler = CatchEvent(renderer,
                                              [&](Event event)
                                              {
                                                  if (event == Event::Return)
                                                  {
                                                      scr.Exit();
                                                      return true;
                                                  }

                                                  if (event == Event::Escape)
                                                  {
                                                      exit = true;
                                                      scr.Exit();
                                                      return true;
                                                  }
                                                  return false;
                                              });
                scr.Loop(key_handler);
                if (exit == true)
                    break;
                continue;
            }

            std::ifstream savefile(systems_path);
            SystemType    placeholder{};
            for (auto &system : systems)
            {
                savefile >> placeholder >> system.get();
            }
            savefile.close();
            map_manager.loadMaps(map_path);
        }
        else
        {
            entity_manager.createEntity(
                EntityType::PLAYER,
                {new WeaponSlot,
                 new WeaponComponent(5),
                 new ArmorSlot,
                 new ArmorComponent(10),
                 new AmuletSlot,
                 new CritComponent,
                 new ExperienceComponent,
                 new BuffComponent,
                 new TileComponent(TileAppearance::PLAYER),
                 new Coordinates,
                 new HungerComponent(101),
                 new Health(10),
                 new Inventory});
            map_manager.generate();
        }

        while (quit == false)
        {
            auto map    = map_manager.getMap();

            auto player = entity_manager.getEntity(1);
            if (option != LaunchOptions::LOAD)
            {

                it_fac.assignMap(&map);
                monster_fac.assignMap(&map);

                uint16_t x{}, y{};
                auto     player = entity_manager.getEntity(1);

                std::uniform_int_distribution<uint16_t> rand_x(1, 98);
                std::uniform_int_distribution<uint16_t> rand_y(1, 48);
                x = rand_x(mt);
                y = rand_y(mt);
                while ((map[x][y].type &
                        (TileType::WALL | TileType::HAS_ITEM |
                         TileType::HAS_CREATURE | TileType::HAS_STAIRS)) !=
                       TileType::NONE)
                {
                    x = rand_x(mt);
                    y = rand_y(mt);
                }
                position_system.assignMap(&map);
                position_system.updatePosition(player, x, y);
                it_fac.generateItems();
                monster_fac.generateMonsters();
            }

            position_system.assignMap(&map);
            nav_map_manager.assignPlayer(player);
            nav_map_manager.assignMap(&map);
            nav_map_manager.calculatePlayerNavMap();
            nav_map_manager.initializeRandomTargets();

            player_control_system.assignPlayer(player);
            player_control_system.assignPositionSystem(&position_system);

            los_system.assignPlayer(player);
            los_system.assignMap(&map);

            ai_system.assignPlayer(player);
            ai_system.assignMap(&map);
            ai_system.assignNavMapManager(&nav_map_manager);

            experience_system.assignPlayer(player);

            entity_manager.readSystemMessages();
            entity_manager.updateData();
            entity_manager.clearSystemMessages();
            position_system.readSystemMessages();
            position_system.updateData();
            position_system.clearSystemMessages();
            effect_system.addEntity(player);

            auto &player_hp = entity_manager.getEntity(1)
                                  ->getComponent<Health>()
                                  ->current_health_points;

            if (option == LaunchOptions::LOAD)
            {
                entity_manager.readSystemMessages();
                entity_manager.updateData();
                entity_manager.clearSystemMessages();
            }
            else
            {
                updateSystems();
                save();
            }

            option = LaunchOptions::NONE;
            GameScreen game_screen(&map, player, position_system, &depth);

            InventoryUI inv_ui(inventory_system);

            auto renderer = Renderer(
                [&] {
                    return game_screen.render(
                        player_control_system.getLastDamagedEnemy());
                });
            // auto renderer = Renderer([&] { return
            // game_screen.debugRender(monster);
            // });

            bool update_systems = false;
            next_level          = false;

            auto player_coords =
                entity_manager.getEntity(1)->getComponent<Coordinates>();

            permissive::fov(
                player_coords->x,
                player_coords->y,
                mask,
                los_system); // so that the map isn't immediately blank

            auto input_handler = CatchEvent(
                renderer,
                [&](Event event)
                {
                    if (event.is_mouse())
                    {
                        update_systems = false;
                        return false;
                    }
                    if (event == Event::Escape)
                    {
                        // scr.Exit();
                        update_systems = false;
                        quit           = true;
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
                    else if (event == Event::ArrowDown)
                    {
                        System::sendSystemMessage(
                            SystemType::PLAYER,
                            {std::make_any<SystemAction::PLAYER>(
                                 SystemAction::PLAYER::MOVE),
                             std::make_any<::Direction>(::Direction::DOWN)});
                        update_systems = true;
                    }
                    else if (event == Event::ArrowLeft)
                    {
                        System::sendSystemMessage(
                            SystemType::PLAYER,
                            {std::make_any<SystemAction::PLAYER>(
                                 SystemAction::PLAYER::MOVE),
                             std::make_any<::Direction>(::Direction::LEFT)});
                        update_systems = true;
                    }
                    else if (event == Event::ArrowRight)
                    {
                        System::sendSystemMessage(
                            SystemType::PLAYER,
                            {std::make_any<SystemAction::PLAYER>(
                                 SystemAction::PLAYER::MOVE),
                             std::make_any<::Direction>(::Direction::RIGHT)});
                        update_systems = true;
                    }
                    else if (event == Event::Character("S") ||
                             event == Event::Character("s"))
                    {
                        std::string save_name = save_screen.render();
                        if (save_name.empty() == false)
                        {
                            save(save_name);
                        }
                    }
                    if (update_systems)
                    {
                        updateSystems();

                        if (next_level || player_hp <= 0)
                        {
                            scr.Exit();
                        }
                        permissive::fov(player_coords->x,
                                        player_coords->y,
                                        mask,
                                        los_system);
                        update_systems = false;
                    }
                    return false;
                });

            auto loop = Loop(&scr, input_handler);

            while (player_hp > 0 && quit == false && next_level == false)
            {
                loop.RunOnce();
            }
            if (player_hp <= 0)
            {
                quit = true;
                inputHighscore();
            }
            if (next_level)
            {
                if (depth == 25)
                {
                    quit = true;
                    inputHighscore();
                }
                else
                {
                    resetSystems();
                    ++depth;
                }
            }
        }
    }
}
