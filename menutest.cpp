#include "src/game/UI/gamescreen.h"
#include "src/game/UI/inventory.h"
#include "src/game/UI/logs.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
int main()
{
    // MainMenu      main_menu;
    int       load = 0;
    LogSystem log_sys;
    Logs      logs(log_sys);

    std::vector<std::string> log_tests = {
        "log1",
        "somewhat long message happens here",
        "very very very very very very very very very very very very very very "
        "very very very very very very very very very very very very very very "
        "very very very very very very very very very very very very very very "
        "very very very very very very very very very very very very very long "
        "messagelog (unlikely)"};
    for (auto &log : log_tests)
    {
        System::sendSystemMessage(SystemType::LOG,
                                  {std::make_any<std::string>(log)});
    }

    log_sys.readSystemMessages();

    // std::cin >> load;
    std::random_device             rd;
    std::mt19937                   mt(rd());
    std::vector<std::vector<Tile>> map(100, std::vector<Tile>(50));
    if (load != 1)
        map = *CaveGenerator::generate(mt, 100, 50);
    else
    {
        std::ifstream is(std::filesystem::current_path() / "map.txt");
        for (int i = 0; i < G_MAP_HEIGHT; ++i)
        {
            for (int j = 0; j < G_MAP_WIDTH; ++j)
            {
                TileType       type;
                TileAppearance appearance;
                is >> type >> appearance;
                map[j][i].type       = type;
                map[j][i].appearance = appearance;
            }
        }
        is.close();
    }
    int depth = 0;

    EntityManager  entity_manager;
    PositionSystem pos_sys(map);
    EffectSystem   eff_sys;
    if (load != 1)
    {
        ItemFactory    it_fac(map, depth);
        MonsterFactory mon_fac(it_fac, map, depth);

        auto player =
            new Entity(EntityType::PLAYER,
                       {
                           new Name("Player"),
                           new WeaponComponent(19),
                           new ArmorComponent(10),
                           new AmuletSlot,
                           new Inventory,
                           new BuffComponent({
                               new EffectComponent(Effect::BLEED, 4, 10),
                           }),
                           new Health(10, 4),
                           new HungerComponent(73, 100),
                           new ExperienceComponent(1, 23),
                       });
        auto weapon = it_fac.generateWeapon(50);
        auto armor  = it_fac.generateArmor(10);
        auto ring   = it_fac.generateRing(14);

        weapon->getComponent<ItemComponent>()->equipped = true;
        armor->getComponent<ItemComponent>()->equipped  = true;
        ring->getComponent<ItemComponent>()->equipped   = true;

        player->addComponent(new WeaponSlot(weapon));
        player->addComponent(new ArmorSlot(armor));

        player->getComponent<AmuletSlot>()->amulet_slots.emplace(ring);
        player->getComponent<AmuletSlot>()->amount_equipped += 1;
        entity_manager.addEntity(player);
        auto inventory = player->getComponent<Inventory>();
        inventory->inventory.push_back(weapon);
        inventory->inventory.push_back(armor);
        inventory->inventory.push_back(ring);

        eff_sys.addEntity(player);
        eff_sys.addEffects(player, ring->getComponent<BuffComponent>());
        it_fac.generateItems();
        mon_fac.generateMonsters();
    }
    else
    {
        std::ifstream is(std::filesystem::current_path() / "entities.txt");
        std::string   placeholder{};

        is >> placeholder;
        // std::cout << placeholder << std::endl;
        is >> entity_manager;
        is >> placeholder;
        // std::cout << placeholder << std::endl;
        is >> pos_sys;
        is >> placeholder;
        // std::cout << placeholder << std::endl;
        is >> eff_sys;
    }

    entity_manager.readSystemMessages();
    // std::cout << entity_manager;
    entity_manager.updateData();
    entity_manager.clearSystemMessages();

    pos_sys.readSystemMessages();
    pos_sys.updateData();
    pos_sys.clearSystemMessages();

    if (load != 1)
    {
        std::ofstream of("entities.txt");
        of << entity_manager << pos_sys << '\n' << eff_sys;
        of.close();

        std::ofstream map_of("map.txt");
        for (int i = 0; i < G_MAP_HEIGHT; ++i)
        {
            for (int j = 0; j < G_MAP_WIDTH; ++j)
            {
                map_of << map[j][i].type << ' ' << map[j][i].appearance << ' ';
            }
            map_of << '\n';
        }
        map_of.close();
    }

    auto       player = entity_manager.getEntity(1);
    GameScreen game_screen(&map, player, pos_sys);
    auto       scr      = ftxui::ScreenInteractive::Fullscreen();
    auto       renderer = ftxui::Renderer(
        [&]
        {
            // return ftxui::hbox({
            //            game_screen.getSidebar(),
            //            ftxui::separatorLight(),
            //            game_screen.getMapBox(),
            //        }) |
            //        ftxui::borderRounded | ftxui::center | ftxui::flex_shrink;
            return game_screen.render();
        });

    auto eventhandler = ftxui::CatchEvent(renderer,
                                          [&](ftxui::Event event)
                                          {
                                              if (event == ftxui::Event::Escape)
                                              {
                                                  scr.Exit();
                                                  return true;
                                              }
                                              return false;
                                          });
    auto loop         = ftxui::Loop(&scr, eventhandler);
    // loop.Run();
    logs.open();
}
