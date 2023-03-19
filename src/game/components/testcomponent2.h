#ifndef TESTCOMPONENT2_H
#define TESTCOMPONENT2_H
#include "../component.h"
#include <iostream>

class TestComponent2 : public Component
{
    void update(Entity *entity = nullptr)
    {
        std::cout << "PUUU MMMMM" << std::endl;
    }
};
#endif /*TESTCOMPONENT2_H*/
