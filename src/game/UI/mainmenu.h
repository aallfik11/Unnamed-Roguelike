#ifndef MAINMENU_H
#define MAINMENU_H
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
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
public:
    void renderMainMenu(LaunchOptions &launch_options)
    {
        using namespace ftxui;
        auto screen     = ScreenInteractive::Fullscreen();
        int  menu_index = 0;
        auto container  = Container::Vertical(
            {
                MenuEntry("New Game"),
                MenuEntry("Load Game"),
                MenuEntry("High Scores"),
                MenuEntry("Exit"),
            },
            &menu_index);
        auto inputHandler =
            CatchEvent(container,
                       [&](Event event)
                       {
                           if (event.is_mouse() == true)
                           {
                               return false;
                           }
                           if (event == Event::ArrowUp)
                           {
                               if (menu_index > 0)
                               {
                                   --menu_index;
                               }
                           }
                           if (event == Event::ArrowDown)
                           {
                               if (menu_index < 2)
                               {
                                   ++menu_index;
                               }
                           }

                           if (event == Event::Return)
                           {
                               if (menu_index < 1)
                               {
                                   launch_options =
                                       LaunchOptions(menu_index + 1);
                                   screen.Exit();
                                   return true;
                               }
                               if (menu_index == 2)
                               {
                                   renderHighScores();
                                   return true;
                               }
                               if (menu_index == 3)
                               {
                                   launch_options = EXIT;
                                   return true;
                                   screen.Exit();
                               }
                           }
                           return false;
                       });
        screen.Loop(inputHandler);
    }
    void renderHighScores()
    {
        using namespace ftxui;
        auto hs_screen = ScreenInteractive::Fullscreen();

        std::ifstream highscore_file("highscores.txt");
        std::string   nick{};
        std::string   score{};
        auto getHighscores = [&](){
            Components highscores;
            while(highscore_file>>nick>>score)
            {
                highscores.emplace_back(hbox({text(nick), text(" " + score)})->Render());
            }
        }
        auto          hs_renderer = Renderer(
            [&]
            {
                Elements highscores;
                while (highscore_file >> nick >> score)
                {
                    highscores.emplace_back(hbox({text(nick), text(score)}));
                }
                return vbox({text("Highscores") | hcenter,
                             vbox({highscores | hcenter})});
            });

    }
};
#endif /*MAINMENU_H*/
