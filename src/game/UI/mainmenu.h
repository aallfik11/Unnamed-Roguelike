#ifndef MAINMENU_H
#define MAINMENU_H
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <map>
#include <string>
enum LaunchOptions
{
    NONE,
    LAUNCH,
    LOAD,
    EXIT
};
class MainMenu
{

    std::string rg_l1, rg_l2, rg_l3, rg_l4, rg_l5, rg_l6, rg_l7;

public:
    MainMenu()
    {
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
    }
    void renderMainMenu(LaunchOptions &launch_options)
    {
        launch_options = LaunchOptions::NONE;
        using namespace ftxui;
        auto screen  = ScreenInteractive::Fullscreen();
        auto rg_text = Renderer(
            [&]()
            {
                return vbox(text(rg_l1),
                            text(rg_l2),
                            text(rg_l3),
                            text(rg_l4),
                            text(rg_l5),
                            text(rg_l6),
                            text(rg_l7)) |
                       hcenter;
            });
        int menu_index = 0;

        MenuEntryOption option;
        option.animated_colors.foreground.enabled = false;
        option.animated_colors.background.enabled = false;

        auto container                            = Container::Vertical(
            {
                MenuEntry("New Game", option) | hcenter,
                MenuEntry("Load Game", option) | hcenter,
                MenuEntry("High Scores", option) | hcenter,
                MenuEntry("Exit", option) | hcenter,
            },
            &menu_index);
        auto main_menu = Renderer(
            [&] {
                return vbox({rg_text->Render(), container->Render() | center});
            });

        auto inputHandler =
            CatchEvent(main_menu,
                       [&](Event event)
                       {
                           if (event != Event::Return)
                           {
                               return container->OnEvent(event);
                           }
                           else if (menu_index <= 1)
                           {
                               launch_options = LaunchOptions(menu_index + 1);
                               screen.Exit();
                           }
                           else if (menu_index == 2)
                           {
                               renderHighScores();
                           }
                           else if (menu_index == 3)
                           {
                               launch_options = EXIT;
                               screen.Exit();
                           }
                           return true;
                       });
        // screen.Loop(inputHandler);
        auto menu_loop = Loop(&screen, inputHandler);
        while (launch_options == LaunchOptions::NONE)
        {
            menu_loop.RunOnceBlocking();
        }
    }

    void renderHighScores()
    {
        using namespace ftxui;
        auto      hs_screen = ScreenInteractive::Fullscreen();
        Component hs_renderer;

        std::ifstream highscore_file("highscores.txt");
        std::multimap<uint64_t, std::string, std::greater<uint64_t>> score_map;
        std::string                                                  nick{};
        uint64_t                                                     score{};

        if (highscore_file.is_open() == false)
        {
            hs_renderer = Renderer(
                []
                {
                    return vbox({paragraphAlignCenter(
                               "No highscores found. Try "
                               "playing the game first :)")}) |
                           center;
                });
        }
        else
        {
            while (highscore_file >> nick >> score)
            {
                score_map.insert({score, nick});
            }
            highscore_file.close();

            hs_renderer = Renderer(
                [&]
                {
                    int      i = 0;
                    Elements top10;
                    for (auto score_it = score_map.begin();
                         i < 10 && score_it != score_map.end();
                         ++score_it, ++i)
                    {
                        top10.push_back(
                            hbox({text(score_it->second),
                                  text(std::to_string(score_it->first))}));
                    }
                    return vbox({text("Top 10 Highscores"), vbox({top10})}) |
                           center;
                });
        }
        auto hs_input_handler = CatchEvent(
            hs_renderer,
            [&](Event event)
            {
                if (event == Event::Escape || event == Event::Character('q'))
                {
                    hs_screen.Exit();
                    return true;
                }
                return false;
            });
        hs_screen.Loop(hs_input_handler);
    }
};
#endif /*MAINMENU_H*/
