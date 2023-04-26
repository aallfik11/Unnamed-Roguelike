#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include <istream>
#include <memory>
#include <ostream>
#include <string>

class Name : public Component
{
    Name         *cloneImpl() const override { return new Name(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::NAME << ' ' << this->name << ' ';
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        is >> this->name;
        return is;
    }

public:
    std::string name;
    Name(std::string name = "") { this->name = name; }
    Name(const Name &nm) { this->name = nm.name; }
};

#endif /*NAME_H*/
