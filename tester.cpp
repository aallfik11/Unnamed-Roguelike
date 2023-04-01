#include "src/game/components/components_all.h"
#include "src/game/components/lineofsightcomponent.h"
#include "src/game/entity.h"
#include "src/game/globals.h"
#include "src/game/systems/aisystem.h"
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
    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map = *DebugMapGenerator::generate(mt, 100, 50);
    PositionSystem     pos_sys(map);
    NavMapManager      nav_test(map);
    HealthSystem       health_sys;
    LOS_System         LOS_sys(map);
    AISystem           ai_sys(pos_sys, health_sys, nav_test);

    std::shared_ptr<Entity> entity(new Entity({new TileComponent(),
                                               new Health(100, 100, true),
                                               new Coordinates(33, 33),
                                               new AIComponent(),
                                               new LOSComponent(1000)}));

    std::shared_ptr<Entity> target(new Entity({new TileComponent(),
                                               new Health(100, 100, true),
                                               new Coordinates(89, 23)}));

    std::shared_ptr<NavMapComponent> navmap(new NavMapComponent());
    pos_sys.addEntity(entity);
    health_sys.addEntity(entity);
    entity->addComponent(new NavMapComponent());

    auto target_coords = target->getComponent<Coordinates>();
    auto target_x      = target_coords->x;
    auto target_y      = target_coords->y;

    nav_test.calculateNavMap(entity,
                             {
                                 {target_x, target_y, 0, 1},
    });
    auto nav_map_ptr = entity->getComponent<NavMapComponent>();

    auto nav_map = nav_map_ptr->nav_map;

    LOS_sys.addEntity(entity);
    LOS_sys.addEntity(target);
    LOS_sys.calculateLOS(entity, target);
    std::cout << std::boolalpha
              << entity->getComponent<LOSComponent>()->has_LOS_to_player;

    ai_sys.addEntity(entity);

    // std::cout << a << b;

    ai_sys.runAI(entity, target);

    // using namespace ftxui;
    // auto scr      = ScreenInteractive::Fullscreen();
    // auto renderer = Renderer(
    //     [&]
    //     {
    //         auto        coord_ptr = entity->getComponent<Coordinates>();
    //         auto        x_coord   = coord_ptr->x;
    //         auto        y_coord   = coord_ptr->y;
    //         std::string sprite    = " ";
    //         Elements    cols;
    //         for (int x = 0; x < 100; x++)
    //         {
    //             Elements rows;
    //             for (int y = 0; y < 50; y++)
    //             {

    //                 auto    score = nav_map[x][y].score;
    //                 uint8_t red   = 0;
    //                 uint8_t green = 0;
    //                 uint8_t blue  = 0;
    //                 if (score != ~0)
    //                 {
    //                     auto score_backup  = score;
    //                     score             /= 3;
    //                     if (score <= 255)
    //                     {
    //                         red   = 255;
    //                         green = score;
    //                     }
    //                     else if (score > 255 && score <= 511)
    //                     {
    //                         red   = 255 - score % 256;
    //                         green = 255;
    //                         blue  = score % 256;
    //                     }
    //                     else if (score > 511 && score <= 767)
    //                     {
    //                         green = 255 - score % 256;
    //                         blue  = 255;
    //                     }
    //                     else
    //                         blue = 255;

    //                     score = score_backup;
    //                 }

    //                 if (x == x_coord && y == y_coord)
    //                 {
    //                     rows.push_back(
    //                         text("@") |
    //                         color(entity->getComponent<TileComponent>()
    //                                   ->tile.color) |
    //                         bgcolor(Color::Black));
    //                 }
    //                 else if (score == 0)
    //                     rows.push_back(text("$") |
    //                                    bgcolor(Color::RGB(red, green,
    //                                    blue)));
    //                 else if (score == ~0)
    //                 {
    //                     rows.push_back(text(" ") | bgcolor(Color::GrayDark));
    //                 }
    //                 else
    //                     rows.push_back(text(std::to_string(score % 9 + 1)) |
    //                                    bgcolor(Color::RGB(red, green,
    //                                    blue)));
    //             }

    //             cols.push_back(vbox(rows));
    //         }
    //         return hbox(cols);
    //     });

    // renderer |= CatchEvent(
    //     [&](Event event)
    //     {
    //         if (event.is_mouse())
    //             return false;
    //         if (event == Event::ArrowDown)
    //         {
    //             auto [x, y] = nav_test.nextBestCoordinates(
    //                 entity, NavMapManager::Destination::TOWARDS);
    //             if (pos_sys.updatePosition(entity, x, y) == false)
    //             {
    //                 entity->getComponent<TileComponent>()->tile.color =
    //                     Color::Red1;
    //             }
    //             return true;
    //         }
    //         if (event == Event::ArrowUp)
    //         {
    //             auto [x, y] = nav_test.nextBestCoordinates(
    //                 entity, NavMapManager::Destination::AWAY_FROM);
    //             if (pos_sys.updatePosition(entity, x, y) == false)
    //             {
    //                 entity->getComponent<TileComponent>()->tile.color =
    //                     Color::Red1;
    //             }
    //             return true;
    //         }
    //         if (event == Event::Tab)
    //         {
    //             scr.Exit();
    //             return true;
    //         }
    //         return false;
    //     });

    // Loop loop(&scr, renderer);

    // while (!loop.HasQuitted())
    // {
    //     loop.RunOnceBlocking();
    // }

    return 0;
}
