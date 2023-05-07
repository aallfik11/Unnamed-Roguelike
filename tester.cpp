#include "src/game/components/components_all.h"
#include "src/game/components/lineofsightcomponent.h"
#include "src/game/entity.h"
#include "src/game/entitytypes.h"
#include "src/game/globals.h"
#include "src/game/systems/aisystem.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/lineofsightsystem.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/navmapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <random>
#include <string>
int main()
{
    std::string rg_text = R"(
  ▄      ▄      ▄   ██   █▀▄▀█ ▄███▄   ██▄       █▄▄▄▄ ████▄   ▄▀    ▄   ▄███▄   █    ▄█ █  █▀ ▄███▄   
   █      █      █  █ █  █ █ █ █▀   ▀  █  █      █  ▄▀ █   █ ▄▀       █  █▀   ▀  █    ██ █▄█   █▀   ▀  
█   █ ██   █ ██   █ █▄▄█ █ ▄ █ ██▄▄    █   █     █▀▀▌  █   █ █ ▀▄  █   █ ██▄▄    █    ██ █▀▄   ██▄▄    
█   █ █ █  █ █ █  █ █  █ █   █ █▄   ▄▀ █  █      █  █  ▀████ █   █ █   █ █▄   ▄▀ ███▄ ▐█ █  █  █▄   ▄▀ 
█▄ ▄█ █  █ █ █  █ █    █    █  ▀███▀   ███▀        █          ███  █▄ ▄█ ▀███▀       ▀ ▐   █   ▀███▀   
 ▀▀▀  █   ██ █   ██   █    ▀                      ▀                 ▀▀▀                   ▀            
                     ▀                                                                                 
)";
    std::string rg_l1, rg_l2, rg_l3, rg_l4, rg_l5, rg_l6, rg_l7;
    rg_l1 = R"(
  ▄      ▄      ▄   ██   █▀▄▀█ ▄███▄   ██▄       █▄▄▄▄ ████▄   ▄▀    ▄   ▄███▄   █    ▄█ █  █▀ ▄███▄   )";
    rg_l2 = R"(
   █      █      █  █ █  █ █ █ █▀   ▀  █  █      █  ▄▀ █   █ ▄▀       █  █▀   ▀  █    ██ █▄█   █▀   ▀  )";
    rg_l3 = R"(
█   █ ██   █ ██   █ █▄▄█ █ ▄ █ ██▄▄    █   █     █▀▀▌  █   █ █ ▀▄  █   █ ██▄▄    █    ██ █▀▄   ██▄▄    )";
    rg_l4 = R"(
█   █ █ █  █ █ █  █ █  █ █   █ █▄   ▄▀ █  █      █  █  ▀████ █   █ █   █ █▄   ▄▀ ███▄ ▐█ █  █  █▄   ▄▀ )";
    rg_l5 = R"(
█▄ ▄█ █  █ █ █  █ █    █    █  ▀███▀   ███▀        █          ███  █▄ ▄█ ▀███▀       ▀ ▐   █   ▀███▀   )";
    rg_l6 = R"(
 ▀▀▀  █   ██ █   ██   █    ▀                      ▀                 ▀▀▀                   ▀            )";
    rg_l7 = R"(
                     ▀                                                                                 )";

    /*
        to do:
        - add additional "checkup maps" to easily check if moves are legal
        - http://www.roguebasin.com/index.php/Data_structures_for_the_map
    */
    std::unique_ptr<Entity> entity(new Entity(EntityType::CREATURE,
                                              {new TileComponent(),
                                               new Health(100, 100, true),
                                               new Coordinates(34, 25),
                                               new AIComponent(),
                                               new LOSComponent(1000),
                                               new BuffComponent,
                                               new CritComponent,
                                               new ArmorComponent,
                                               new WeaponComponent(10)}));

    std::unique_ptr<Entity> target(new Entity(EntityType::PLAYER,
                                              {new TileComponent(),
                                               new WeaponComponent(10),
                                               new CritComponent,
                                               new ArmorComponent(10),
                                               new BuffComponent,
                                               new Health(100, 100, true),
                                               new Coordinates(89, 23)}));

    std::random_device rd;
    std::mt19937       mt(rd());
    // auto               map = *CaveGenerator::generate(mt, 100, 50);
    auto               map = *DebugMapGenerator::generate(mt, 100, 50);
    PositionSystem     pos_sys(map);
    NavMapManager      nav_test(map, target->getComponent<Coordinates>());
    LOS_System         LOS_sys(map, nullptr);
    AISystem           ai_sys(map, nav_test, target.get());
    HealthSystem       hp_sys;
    AttackSystem       att_sys;
    /* for (auto &col : map)
    {
        for (auto &cell : col)
        {
            cell.type |= TileType::VISIBLE;
        }
    } */

    // Tile walltile;
    // walltile.type    = WALL;

    // map[89][22].type = WALL;
    // map[88][22].type = WALL;
    // map[89][24].type = WALL;
    // map[88][23].type = WALL;
    // map[88][24].type = WALL;

    // map[88][22].type = WALL;
    // map[88][21].type = WALL;
    // map[88][20].type = WALL;
    // map[88][19].type = WALL;
    // map[88][18].type = WALL;
    // map[88][17].type = WALL;
    // map[88][16].type = WALL;
    // map[88][15].type = WALL;
    // map[88][14].type = WALL;

    // map[88][23].type = WALL;
    // map[88][24].type = WALL;
    // map[88][25].type = WALL;
    // map[88][26].type = WALL;
    // map[88][27].type = WALL;
    // map[88][28].type = WALL;

    std::shared_ptr<NavMapComponent> navmap(new NavMapComponent());
    pos_sys.addEntity(entity.get());
    entity->addComponent(new NavMapComponent());

    auto target_coords = target->getComponent<Coordinates>();
    auto target_x      = target_coords->x;
    auto target_y      = target_coords->y;

    // nav_test.calculateNavMap(entity,
    //                          {
    //                              {target_x, target_y, 0, 1},
    // });

    nav_test.assignRandomTarget(
        entity->getComponent<NavMapComponent>()->nav_map);
    auto nav_map_ptr = entity->getComponent<NavMapComponent>();

    auto &nav_map    = nav_map_ptr->nav_map;

    LOS_sys.assignPlayer(target.get());
    LOS_sys.addEntity(entity.get());
    LOS_sys.calculateLOS(entity.get(), target.get());
    std::cout << std::boolalpha
              << entity->getComponent<LOSComponent>()->has_LOS_to_player;

    ai_sys.addEntity(entity.get());

    // std::cout << a << b;
    // std::cout << std::endl
    //           << target->getComponent<Health>()->current_health_points;
    // ai_sys.runAI(entity, target);
    // std::cout << std::endl
    //           << "after ai run "
    //           << target->getComponent<Health>()->current_health_points;

    using namespace ftxui;
    auto scr      = ScreenInteractive::Fullscreen();
    auto renderer = Renderer(
        [&]
        {
            auto        coord_ptr     = entity->getComponent<Coordinates>();
            auto        x_coord       = coord_ptr->x;
            auto        y_coord       = coord_ptr->y;
            auto        player_coords = target->getComponent<Coordinates>();
            std::string sprite        = " ";
            Elements    cols;
            for (int x = 0; x < 100; x++)
            {
                Elements rows;
                for (int y = 0; y < 50; y++)
                {

                    auto    score = nav_map[x][y].score;
                    uint8_t red   = 0;
                    uint8_t green = 0;
                    uint8_t blue  = 0;
                    if (score != ~0)
                    {
                        auto score_backup  = score;
                        score             /= 3;
                        if (score <= 255)
                        {
                            red   = 255;
                            green = score;
                        }
                        else if (score > 255 && score <= 511)
                        {
                            red   = 255 - score % 256;
                            green = 255;
                            blue  = score % 256;
                        }
                        else if (score > 511 && score <= 767)
                        {
                            green = 255 - score % 256;
                            blue  = 255;
                        }
                        else
                            blue = 255;

                        score = score_backup;
                    }

                    if (x == player_coords->x && y == player_coords->y)
                    {
                        rows.push_back(
                            text("@") |
                            /* color(entity->getComponent<TileComponent>()
                                      ->tile.color) | */
                            color(Color::Grey15) | bgcolor(Color::Black));
                    }
                    else if (x == x_coord && y == y_coord)
                    {
                        rows.push_back(
                            text("M") |
                            /* color(entity->getComponent<TileComponent>()
                                      ->tile.color) | */
                            color(Color::Grey15) | bgcolor(Color::Black));
                    }
                    else if (score == 0)
                    {
                        if ((map[x][y].type & TileType::WALL) == TileType::NONE)
                            rows.push_back(text("$") | bgcolor(Color::RGB(
                                                           red, green, blue)));
                        else
                            rows.push_back(text("$") |
                                           bgcolor(Color::BlueViolet));
                    }
                    else if (score == ~0)
                    {
                        rows.push_back(text(" ") | bgcolor(Color::GrayDark));
                    }
                    else
                        rows.push_back(text(std::to_string(score % 9 + 1)) |
                                       bgcolor(Color::RGB(red, green, blue)));
                }

                cols.push_back(vbox(rows));
            }
            auto hp = std::to_string(
                target->getComponent<Health>()->current_health_points);
            auto enemy_hp = std::to_string(
                entity->getComponent<Health>()->current_health_points);
            // return vbox(hbox(cols), hbox(text(hp), text(enemy_hp)));
            return vbox(text(rg_l1),
                        text(rg_l2),
                        text(rg_l3),
                        text(rg_l4),
                        text(rg_l5),
                        text(rg_l6),
                        text(rg_l7)) |
                   hcenter;
        });

    renderer |= CatchEvent(
        [&](Event event)
        {
            if (event.is_mouse())
                return false;
            if (event == Event::ArrowDown)
            {
                auto [x, y] = nav_test.nextBestCoordinates(
                    entity.get(), NavMapManager::Destination::TOWARDS);
                if (pos_sys.updatePosition(entity.get(), x, y) == false)
                {
                    /* entity->getComponent<TileComponent>()->tile.color =
                        Color::Red1; */
                }
                return true;
            }
            if (event == Event::ArrowUp)
            {
                auto [x, y] = nav_test.nextBestCoordinates(
                    entity.get(), NavMapManager::Destination::AWAY_FROM);
                if (pos_sys.updatePosition(entity.get(), x, y) == false)
                {
                    /* entity->getComponent<TileComponent>()->tile.color =
                        Color::Red1; */
                }
                return true;
            }
            if (event == Event::Character("w"))
            {
                pos_sys.updatePosition(
                    target.get(), target_coords->x, target_coords->y - 1);
            }
            if (event == Event::Character("a"))
            {

                pos_sys.updatePosition(
                    target.get(), target_coords->x - 1, target_coords->y);
            }
            if (event == Event::Character("s"))
            {
                pos_sys.updatePosition(
                    target.get(), target_coords->x, target_coords->y + 1);
            }
            if (event == Event::Character("d"))
            {
                pos_sys.updatePosition(
                    target.get(), target_coords->x + 1, target_coords->y);
            }
            if (event == Event::Character("x"))
            {
                auto message = {std::make_any<Entity *>(target.get()),
                                std::make_any<Entity *>(entity.get())};
                System::sendSystemMessage(SystemType::ATTACK, message);
            }
            if (event == Event::Tab)
            {
                scr.Exit();
                return true;
            }
            if (event == Event::ArrowLeft)
            {
                ai_sys.runAI(entity.get(), target.get());
                pos_sys.readSystemMessages();
                pos_sys.updateData();
                pos_sys.clearSystemMessages();
                return true;
            }
            LOS_sys.updateData();
            nav_test.updateData();
            ai_sys.runAI(entity.get(), target.get());
            pos_sys.readSystemMessages();
            pos_sys.updateData();
            pos_sys.clearSystemMessages();
            att_sys.readSystemMessages();
            att_sys.updateData();
            att_sys.clearSystemMessages();
            hp_sys.readSystemMessages();
            hp_sys.updateData();
            hp_sys.clearSystemMessages();
            return false;
        });

    Loop loop(&scr, renderer);

    while (!loop.HasQuitted())
    {
        loop.RunOnceBlocking();
    }
    std::cout << std::endl << target.get();

    return 0;
}
