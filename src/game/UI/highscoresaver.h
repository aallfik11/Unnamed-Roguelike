#ifndef HIGHSCORESAVER_H
#define HIGHSCORESAVER_H
#include <ftxui/component/screen_interactive.hpp>

class HighScoreSaver
{
    ftxui::ScreenInteractive &scr_;
    std::string               w_1, w_2, w_3, w_4, w_5, w_6, w_7, w_8;
    std::string               l_1, l_2, l_3, l_4, l_5, l_6, l_7, l_8;

public:
    HighScoreSaver(ftxui::ScreenInteractive &scr);
    void render(uint64_t score, bool win = false);
};

#endif /*HIGHSCORESAVER_H*/
