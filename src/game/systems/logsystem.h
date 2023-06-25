#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H
#include "../system.h"
#include <list>
#include <string>

class LogSystem : public System
{
    friend class GameScreen;
    friend class Logs;
    std::list<std::string> logs_;

public:
    void readSystemMessages() override;
    void updateData() override;
    void clearSystemMessages() override;
    void hardReset();
};

#endif /*LOGSYSTEM_H*/
