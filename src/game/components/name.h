#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include <string>

class Name : public Component
{
public:
    std::string name;
    Name() { name = ""; }

    // void update(Entity *entity) override {}
    // void get() override {}
    // void set() override {}
};

#endif /*NAME_H*/