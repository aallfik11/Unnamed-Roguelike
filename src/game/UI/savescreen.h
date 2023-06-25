#ifndef SAVESCREEN_H
#define SAVESCREEN_H
#include <ftxui/component/screen_interactive.hpp>

class SaveScreen
{
    ftxui::ScreenInteractive &scr_;

public:
    SaveScreen(ftxui::ScreenInteractive &scr); 
    std::string render();
};

#endif /*SAVESCREEN_H*/
