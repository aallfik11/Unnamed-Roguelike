#ifndef LOGS_H
#define LOGS_H
#include "../systems/logsystem.h"
#include <ftxui/component/screen_interactive.hpp>

class Logs
{
    LogSystem                &log_system_;
    ftxui::ScreenInteractive &scr_;

public:
    Logs(LogSystem &log_system, ftxui::ScreenInteractive &scr);
    void open();
};

#endif /*LOGS_H*/
