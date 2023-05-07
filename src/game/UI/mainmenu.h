#ifndef MAINMENU_H
#define MAINMENU_H
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
enum LaunchOptions
{
    NONE   = 0x00,
    LAUNCH = 0x01,
    LOAD   = 0x02,
};
class MainMenu
{
    void renderMainMenu(LaunchOptions& launch_options) {
        using namespace ftxui;
        auto screen = ScreenInteractive::Fullscreen();
        auto container = Container::Vertical({});
    }
    void renderSettings() {}
    void renderHighScores() {}
};
#endif /*MAINMENU_H*/
