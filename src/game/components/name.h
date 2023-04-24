#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include <memory>
#include <string>

class Name : public Component
{
    Name *cloneImpl() const override { return new Name(*this); }

public:
    std::string name;
    Name(std::string name = "") { this->name = name; }
    Name(const Name &nm) { this->name = nm.name; }

    // std::unique_ptr<Name> clone() const
    // {
    //     return std::unique_ptr<Name>(this->cloneImpl());
    // }
};

#endif /*NAME_H*/
