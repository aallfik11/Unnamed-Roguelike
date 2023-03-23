#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include <string>

class Name : public Component
{
public:
    std::string name;
    Name(std::string name = "")
    {
        this->name = name;
    }
};

#endif /*NAME_H*/