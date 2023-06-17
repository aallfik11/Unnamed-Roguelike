#ifndef HIGHSCORESAVER_H
#define HIGHSCORESAVER_H
#include <filesystem>
#include <fstream>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <regex>

class HighScoreSaver
{
    ftxui::ScreenInteractive &scr_;
    std::string               w_1, w_2, w_3, w_4, w_5, w_6, w_7, w_8;
    std::string               l_1, l_2, l_3, l_4, l_5, l_6, l_7, l_8;

public:
    HighScoreSaver(ftxui::ScreenInteractive &scr) : scr_{scr}
    {

        w_1 = R"(▀▄    ▄ ████▄   ▄         ▄ ▄   ▄█    ▄)";
        w_2 = R"(  █  █  █   █    █       █   █  ██     █)";
        w_3 = R"(   ▀█   █   █ █   █     █ ▄   █ ██ ██   █)";
        w_4 = R"(   █    ▀████ █   █     █  █  █ ▐█ █ █  █)";
        w_5 = R"( ▄▀           █▄ ▄█      █ █ █   ▐ █  █ █)";
        w_6 = R"(               ▀▀▀        ▀ ▀      █   ██)";
        w_7 = R"()";
        w_8 = R"()";

        l_1 = R"(▓██   ██▓ ▒█████   █    ██    ▓█████▄  ██▓▓█████ ▓█████▄)";
        l_2 = R"( ▒██  ██▒▒██▒  ██▒ ██  ▓██▒   ▒██▀ ██▌▓██▒▓█   ▀ ▒██▀ ██▌)";
        l_3 = R"(  ▒██ ██░▒██░  ██▒▓██  ▒██░   ░██   █▌▒██▒▒███   ░██   █▌)";
        l_4 = R"(  ░ ▐██▓░▒██   ██░▓▓█  ░██░   ░▓█▄   ▌░██░▒▓█  ▄ ░▓█▄   ▌)";
        l_5 = R"(  ░ ██▒▓░░ ████▓▒░▒▒█████▓    ░▒████▓ ░██░░▒████▒░▒████▓)";
        l_6 = R"(   ██▒▒▒ ░ ▒░▒░▒░ ░▒▓▒ ▒ ▒     ▒▒▓  ▒ ░▓  ░░ ▒░ ░ ▒▒▓  ▒)";
        l_7 = R"( ▓██ ░▒░   ░ ▒ ▒░ ░░▒░ ░ ░     ░ ▒  ▒  ▒ ░ ░ ░  ░ ░ ▒  ▒)";
        l_8 = R"( ▒ ▒ ░░  ░ ░ ░ ▒   ░░░ ░ ░     ░ ░  ░  ▒ ░   ░    ░ ░  ░)";
    }
    void render(uint64_t score, bool win = false)
    {
        static std::regex pattern("^[a-zA-Z0-9]{1,20}$");

        bool        invalid_nickname = false;
        bool        quit             = false;
        std::string nickname{};
        std::string message         = "Input Your Nickame";
        std::string invalid_message = "Invalid Nickname";

        using namespace ftxui;

        Element end_message;
        if (win == true)
        {
            end_message = vbox({
                text(w_1),
                text(w_2),
                text(w_3),
                text(w_4),
                text(w_5),
                text(w_6),
                text(w_7),
                text(w_8),
            });
        }
        else
        {
            end_message = vbox({
                text(l_1),
                text(l_2),
                text(l_3),
                text(l_4),
                text(l_5),
                text(l_6),
                text(l_7),
                text(l_8),
            });
        }
        auto normal_color  = Color::White;
        auto invalid_color = Color::Red;
        auto input = Container::Vertical({Input(&nickname, "Nickname")});

        auto flexbox_config = FlexboxConfig();
        flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);
        auto controls =
            flexbox(
                {
                    text("ESC - Don't Save Highscore") | borderRounded,
                    text("ENTER - Confirm Nickname") | borderRounded,
                },
                flexbox_config) |
            hcenter;

        auto renderer = ftxui::Renderer(
            [&]
            {
                Color       text_color     = normal_color;
                std::string window_message = message;
                if (invalid_nickname)
                {
                    text_color     = invalid_color;
                    window_message = invalid_message;
                }
                input->TakeFocus();
                return vbox(
                    {end_message | center,
                     separatorEmpty(),
                     text("Your Score: " + std::to_string(score)) | center,
                     separatorEmpty(),
                     window(text(message) | color(text_color),
                            input->Render()) |
                         center | flex_grow,
                     controls});
            });

        auto input_handler = CatchEvent(
            renderer,
            [&](Event event)
            {
                if (event == Event::Return)
                {
                    if (std::regex_match(nickname, pattern))
                    {
                        std::fstream hs_file("highscores.txt", std::ios::app);
                        hs_file << nickname << ' ' << score;
                        quit = true;
                        scr_.Exit();
                    }
                    else
                    {
                        invalid_nickname = true;
                    }
                    return true;
                }
                if (event == Event::Escape)
                {
                    quit = true;
                    scr_.Exit();
                    return true;
                }
                input->TakeFocus();
                return input->OnEvent(event);
            });

        auto loop = Loop(&scr_, input_handler);
        while (quit != true)
        {
            loop.RunOnce();
        }
    }
};

#endif /*HIGHSCORESAVER_H*/
