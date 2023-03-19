#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H
#include "../component.h"
#include <string>

class TestComponent : public Component
{
    public:
    std::string test = "TEST";
    // void update(Entity *entity = nullptr)
    // {
    //     std::cout << "MMMM PUUUUU" << std::endl;
    // }
    // void get() {}
    // void set() {}
};

#endif /*TESTCOMPONENT_H*/
