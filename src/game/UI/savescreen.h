#ifndef SAVESCREEN_H
#define SAVESCREEN_H
#include <filesystem>
#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <regex>

class SaveScreen
{
    ftxui::ScreenInteractive &scr_;

public:
    SaveScreen(ftxui::ScreenInteractive &scr) : scr_{scr} {}
    std::string render()
    {
        static std::regex pattern("^(?!autosave$)[a-zA-Z0-9]+$");

        bool        invalid_filename = false;
        bool        quit             = false;
        std::string filename{};
        std::string message         = "Input a Savefile Name";
        std::string invalid_message = "File Name not Allowed";

        using namespace ftxui;

        auto normal_color  = Color::White;
        auto invalid_color = Color::Red;
        auto input = Container::Vertical({Input(&filename, "Savefile Name")});

        auto flexbox_config = FlexboxConfig();
        flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);
        auto controls =
            flexbox(
                {
                    text("ESC - Cancel Savefile Creation") | borderRounded,
                    text("ENTER - Confirm Save") | borderRounded,
                },
                flexbox_config) |
            hcenter;

        auto renderer = ftxui::Renderer(
            [&]
            {
                Color       text_color     = normal_color;
                std::string window_message = message;
                if (invalid_filename)
                {
                    text_color     = invalid_color;
                    window_message = invalid_message;
                }
                input->TakeFocus();
                return vbox({window(text(message) | color(text_color),
                                    input->Render()) |
                                 center | flex_grow,
                             controls});
            });

        auto input_handler =
            CatchEvent(renderer,
                       [&](Event event)
                       {
                           if (event == Event::Return)
                           {
                               if (std::regex_match(filename, pattern))
                               {
                                   quit = true;
                                   scr_.Exit();
                               }
                               else
                               {
                                   invalid_filename = true;
                               }
                               return true;
                           }
                           if (event == Event::Escape)
                           {
                               quit = true;
                               filename.clear();
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
        return filename;
    }
};

#endif /*SAVESCREEN_H*/
