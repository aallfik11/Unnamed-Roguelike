#ifndef LOGS_H
#define LOGS_H
#include "../systems/logsystem.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

class Logs
{
    LogSystem                &log_system_;
    ftxui::ScreenInteractive &scr_;

public:
    Logs(LogSystem &log_system, ftxui::ScreenInteractive &scr)
        : log_system_{log_system}, scr_{scr}
    {
    }
    void open()
    {
        using namespace ftxui;
        Components messages;
        for (const auto &message : log_system_.logs_)
        {
            messages.emplace_back(MenuEntry(message));
        }
        auto        container      = Container::Vertical(messages);
        static auto flexbox_config = FlexboxConfig();
        flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);
        static auto controls =
            flexbox(
                {
                    text("C - Clear All Logs") | borderRounded,
                },
                flexbox_config) |
            hcenter;

        auto renderer = Renderer(
            [&]
            {
                if (messages.empty())
                {
                    return text("Logs Empty");
                }
                return vbox({container->Render() | vscroll_indicator | frame |
                                 flex_grow,
                             separatorLight(),
                             controls});
            });

        auto key_handler =
            CatchEvent(renderer,
                       [&](Event event)
                       {
                           if (event == Event::Escape)
                           {
                               scr_.Exit();
                               return true;
                           }
                           else if (event == Event::Character("C") ||
                                    event == Event::Character("c"))
                           {
                               log_system_.hardReset();
                           }
                           return container->OnEvent(event);
                       });
        scr_.Loop(key_handler);
    }
};

#endif /*LOGS_H*/
