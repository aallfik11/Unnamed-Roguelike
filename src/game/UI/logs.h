#ifndef LOGS_H
#define LOGS_H
#include "../systems/logsystem.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

class Logs
{
    LogSystem &log_system_;

public:
    Logs(LogSystem &log_system) : log_system_{log_system} {}
    void open()
    {
        using namespace ftxui;
        auto       scr = ScreenInteractive::Fullscreen();
        Components messages;
        for (const auto &message : log_system_.logs_)
        {
            messages.emplace_back(MenuEntry(message));
        }
        auto container   = Container::Vertical(messages);

        auto key_handler = CatchEvent(container,
                                      [&](Event event)
                                      {
                                          if (event == Event::Escape)
                                          {
                                              scr.Exit();
                                              return true;
                                          }
                                          return container->OnEvent(event);
                                      });
        scr.Loop(key_handler);
    }
};

#endif /*LOGS_H*/
