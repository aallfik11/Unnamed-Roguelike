#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include <algorithm>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

class Name : public Component
{
    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto n = static_cast<const Name*>(c);
        return (this->name == n->name);
    }
    Name         *cloneImpl() const override { return new Name(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        auto word_count =
            std::count(this->name.cbegin(), this->name.cend(), ' ') + 1;
        os << ComponentType::NAME << ' ' << word_count << ' ' << this->name
           << ' ';
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        int         word_count{};
        std::string temp{};
        is >> word_count;
        for (int i = 0; i < word_count - 1; i++)
        {
            is >> temp;
            this->name += temp + " ";
        }
        is >> temp;
        this->name += temp;
        return is;
    }

public:
    std::string name;
    Name(std::string name = "") { this->name = name; }
    Name(const Name &nm) { this->name = nm.name; }
};

#endif /*NAME_H*/
