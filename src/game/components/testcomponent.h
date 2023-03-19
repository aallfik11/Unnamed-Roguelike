#ifndef TESTCOMPONENT_H
#define TESTCOMPONENT_H
#include "../component.h"
#include <string>

class TestComponent : public Component
{
public:
    // void update(Entity *entity = nullptr)
    // {
    //     std::cout << "MMMM PUUUUU" << std::endl;
    // }
    // void get() {}
    // void set() {}
    std::string test = "PUU";
};
#endif /*TESTCOMPONENT_H*/
