#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include <memory>
#include <string>

class Description : public Component
{

public:
    std::string description;
    Description(std::string description = "")
    {
        this->description = description;
    }
    Description(const Description &desc)
    {
        this->description = desc.description;
    }
    Description *clone() const override
    {
        return new Description(*this);
    }
    // std::unique_ptr<Description> clone() const
    // {
    //     return std::unique_ptr<Description>(this->cloneImpl());
    // }
};

#endif /*DESCRIPTION_H*/
