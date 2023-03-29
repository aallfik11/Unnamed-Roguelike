#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include <string>

class Description : public Component
{
public:
    std::string description;
    Description(std::string description = "")
    {
        this->description = description;
    }
};

#endif /*DESCRIPTION_H*/