#include "mainmenu.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <map>
#include <string>

MainMenu::MainMenu(ftxui::ScreenInteractive &scr) : scr_{scr}
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
void MainMenu::renderMainMenu(LaunchOptions &launch_options,
                              std::string   &savefile_path)
{
    launch_options = LaunchOptions::NONE;
    using namespace ftxui;
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
                       else if (menu_index == 0)
                       {
                           launch_options = LaunchOptions::LAUNCH;
                           scr_.Exit();
                       }
                       else if (menu_index == 1)
                       {
                           renderSaves(savefile_path);
                           if (savefile_path.empty())
                           {
                               launch_options = LaunchOptions::NONE;
                           }
                           else
                           {
                               launch_options = LaunchOptions::LOAD;
                               scr_.Exit();
                           }
                       }
                       else if (menu_index == 2)
                       {
                           renderHighScores();
                       }
                       else if (menu_index == 3)
                       {
                           launch_options = EXIT;
                           scr_.Exit();
                       }
                       return true;
                   });
    // screen.Loop(inputHandler);
    auto menu_loop = Loop(&scr_, inputHandler);
    while (launch_options == LaunchOptions::NONE)
    {
        menu_loop.RunOnceBlocking();
    }
}

void MainMenu::renderHighScores()
{
    using namespace ftxui;
    ftxui::Component hs_renderer;

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
        Color top1          = Color::Red;
        Color top2          = Color::Yellow;
        Color top3          = Color::LightGreen;
        Color other         = Color::White;

        auto flexbox_config = FlexboxConfig();
        flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceAround);

        hs_renderer = Renderer(
            [&]
            {
                int      i = 0;
                Elements top10;
                for (auto score_it = score_map.begin();
                     i < 10 && score_it != score_map.end();
                     ++score_it, ++i)
                {
                    Color text_color = other;
                    if (i == 0)
                        text_color = top1;
                    if (i == 1)
                        text_color = top2;
                    if (i == 2)
                        text_color = top3;
                    top10.push_back(
                        flexbox({text(score_it->second + " "),
                                 text(std::to_string(score_it->first)) |
                                     color(text_color)},
                                flexbox_config) |
                        hcenter);
                }
                return vbox({text("Top 10 Highscores"), vbox({top10})}) |
                       center;
            });
    }
    auto hs_input_handler = CatchEvent(hs_renderer,
                                       [&](Event event)
                                       {
                                           if (event == Event::Escape ||
                                               event == Event::Character('q'))
                                           {
                                               scr_.Exit();
                                               return true;
                                           }
                                           return false;
                                       });
    scr_.Loop(hs_input_handler);
}

void MainMenu::renderSaves(std::string &savefile_path)
{
    using namespace ftxui;
    using namespace std::filesystem;
    auto general_save_path = current_path() / path("saves");
    if (exists(general_save_path) == false)
    {
        create_directory(general_save_path);
    }

    int        selector = 0;
    Components saves;
    auto       save_container = Container::Vertical(Components(), &selector);
    auto       save_iterator  = directory_iterator();
    auto       flexbox_config = FlexboxConfig();
    flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);

    auto renderer = Renderer(
        [&]
        {
            if (is_empty(general_save_path))
            {
                return text("No savefiles present, try playing the game "
                            "first :)") |
                       flex_shrink | borderLight | center;
            }
            saves.clear();
            save_container->DetachAllChildren();
            save_iterator = directory_iterator(general_save_path);
            while (save_iterator != directory_iterator{})
            {
                if (save_iterator->is_directory())
                    saves.push_back(
                        MenuEntry(save_iterator->path().filename().string()));
                ++save_iterator;
            }
            for (auto &save : saves)
            {
                save_container->Add(save);
            }

            return vbox({save_container->Render() | center | flex_grow,
                         flexbox({text("ENTER - Select Save"),
                                  text("D - Delete Save"),
                                  text("ESCAPE - Exit this screen")},
                                 flexbox_config)});
        });

    auto key_handler = CatchEvent(
        renderer,
        [&](Event event)
        {
            selector = std::max(
                0, std::min(static_cast<int>(saves.size() - 1), selector));
            save_iterator = directory_iterator(general_save_path);
            std::advance(save_iterator, selector);
            if (event == Event::Return)
            {
                savefile_path = save_iterator->path().filename().string();
                scr_.Exit();
            }
            else if (event == Event::Escape)
            {
                savefile_path.clear();
                scr_.Exit();
            }
            else if (event.character() == "d" || event.character() == "D")
            {
                remove_all(save_iterator->path());
            }
            return save_container->OnEvent(event);
        });
    scr_.Loop(key_handler);
}
