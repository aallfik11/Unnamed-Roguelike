#include "logsystem.h"
#include <any>
#include <list>
#include <string>

void LogSystem::readSystemMessages()
{
    for (auto &message : (*system_messages_)[SystemType::LOG])
    {
        auto message_it = message.begin();
        logs_.emplace_back(std::any_cast<std::string>(*message_it));
    }
}
void LogSystem::updateData() {}
void LogSystem::clearSystemMessages()
{
    (*system_messages_)[SystemType::LOG].clear();
}
void LogSystem::hardReset() { logs_.clear(); }
