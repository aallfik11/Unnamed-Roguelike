#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include "../system.h"
#include <any>
#include <list>
#include <string>

class Logs;

class LogSystem : public System
{
    friend class GameScreen;
    std::list<std::string> logs_;

public:
    void readSystemMessages() override
    {
        for (auto &message : (*system_messages_)[SystemType::LOG])
        {
            auto message_it = message.begin();
            logs_.emplace_back(std::any_cast<std::string>(*message_it));
        }
    }
    void updateData() override {}
    void clearSystemMessages() override {}
};

#endif /*LOGSYSTEM_H*/