#include "src/game/component.h"
#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/globals.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/navmapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <random>

int main()
{
    std::random_device rd;
    std::mt19937 mt(rd());
    auto map = *CaveGenerator::generate(mt, 100, 50);

    // map[10][10].type = WALL;

    // map[10][11].type = WALL;
    // map[10][12].type = WALL;
    // map[10][13].type = WALL;
    NavMapManager nav_test(map);
    std::shared_ptr<Entity> entity(new Entity());
    std::shared_ptr<NavMapComponent> navmap(new NavMapComponent());
    entity->addComponent(navmap);
    // entity->addComponent(new NavMapComponent());

    int breakpoint;

    nav_test.calculateNavMap(entity, {
        {5, 5, 0, 1.0},
        {67, 30, 3, .2}
        });
    auto nav_map_ptr = entity->getComponent<NavMapComponent>();

    auto nav_map = nav_map_ptr->nav_map;

    // using namespace ftxui;
    // auto scr = ScreenInteractive::Fullscreen();
    // auto renderer = Renderer([&]
    //                          {
    //                              Elements cols;
    //                              for (int x = 0; x < 100; x++)
    //                              {
    //                                  Elements rows;
    //                                  for (int y = 0; y < 50; y++)
    //                                  {
    //                                      auto score = nav_map[x][y].score;
    //                                      uint8_t red = 0;
    //                                      uint8_t green = 0;
    //                                      uint8_t blue = 0;
    //                                      if (score <= 255)
    //                                      {
    //                                          red = 255;
    //                                          green = score;
    //                                      }
    //                                      else if (score > 255 && score <= 511)
    //                                      {
    //                                          red = 255 - score % 256;
    //                                          green = 255;
    //                                          blue = score % 256;
    //                                      }
    //                                      else if (score > 511 && score <= 767)
    //                                      {
    //                                          green = 255 - score % 256;
    //                                          blue = 255;
    //                                      }
    //                                      else
    //                                          blue = 255;
                                         
    //                                      if (score == 0)
    //                                         rows.push_back(text("$") | bgcolor(Color::RGB(red, green, blue)));
    //                                         else if(score == ~0)
    //                                         {
    //                                             rows.push_back(text(" ") | bgcolor(Color::GrayDark));
    //                                         } else
    //                                          rows.push_back(text(".") | bgcolor(Color::RGB(red, green, blue)));
    //                                  }
    //                                  cols.push_back(vbox(rows));
    //                              }
    //                              return hbox(cols); });

    // scr.Loop(renderer);

    return 0;
}
