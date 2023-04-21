#ifndef PLAYERCONTROLSYSTEM_H
#define PLAYERCONTROLSYSTEM_H
#include "../system.h"
#include <ftxui/component/event.hpp>
#include <list>

class PlayerControlSystem : public System
{

    inline void determineNextAction(const ftxui::Event &event)
    {

        using namespace ftxui;
        if (event.is_character())
        {
            // open inventory
            if (event == Event::Character("i") ||
                event == Event::Character("I"))
            {
            }

            // rest
            if (event == Event::Character("r") ||
                event == Event::Character("R"))
            {
                
            }

            // logs
            if (event == Event::Character("l") ||
                event == Event::Character("L"))
            {
            }

            // save
            if (event == Event::Character("s") ||
                event == Event::Character("S"))
            {
                /*SAVE SYSTEM GOES HERE*/
            }
        }
        if (event == Event::ArrowDown)
        {
        }
        if (event == Event::ArrowUp)
        {
        }
        if (event == Event::ArrowLeft)
        {
        }
        if (event == Event::ArrowRight)
        {
        }
    }
    void openInventory() {}

public:
    void updateData() override {}
    void readSystemMessages() override {}
    void clearSystemMessages() override {}
};

#endif /*PLAYERCONTROLSYSTEM_H*/
