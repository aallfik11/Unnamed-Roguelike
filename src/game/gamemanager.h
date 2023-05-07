#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H
#include "UI/gamescreen.h"
#include "UI/inventory.h"
#include "UI/mainmenu.h"
#include <ftxui/component/screen_interactive.hpp>

class GameManager
{
    LaunchOptions launch_options_;
    MainMenu main_menu_;

    void saveGame() {}
    void loadGame() {}
    void initializeGame() {}
    void renderGame() {}
    void saveHighscore() {}
    void gameLoop() {}

public:
    GameManager() { launch_options_ = LaunchOptions::NONE; }
    void menuLoop()
    {
        using namespace ftxui;
        auto screen = ScreenInteractive::Fullscreen();
        main_menu_.renderMainMenu(launch_options_);
    }
};

#endif /*GAMEMANAGER_H*/
