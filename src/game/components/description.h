#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include <memory>
#include <string>

class Description : public Component
{
    Description *cloneImpl() const override
    {
        return new Description(this->description);
    }

public:
    std::string description;
    Description(std::string description = "")
    {
        this->description = description;
    }

    std::unique_ptr<Description> clone() const
    {
        return std::unique_ptr<Description>(this->cloneImpl());
    }
};

#endif /*DESCRIPTION_H*/