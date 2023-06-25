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

    std::string               rg_l1, rg_l2, rg_l3, rg_l4, rg_l5, rg_l6, rg_l7;
    ftxui::ScreenInteractive &scr_;

public:
    MainMenu(ftxui::ScreenInteractive &scr);

    void renderMainMenu(LaunchOptions &launch_options,
                        std::string   &savefile_path);
    void renderHighScores();
    void renderSaves(std::string &savefile_path);
};
#endif /*MAINMENU_H*/
