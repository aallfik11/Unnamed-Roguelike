#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include <istream>
#include <memory>
#include <ostream>
#include <string>

class Description : public Component
{

    Description  *cloneImpl() const override { return new Description(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::DESCRIPTION << ' ' << this->description << ' ';
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        is >> this->description;
        return is;
    }

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
};

#endif /*DESCRIPTION_H*/
