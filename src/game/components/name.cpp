#include "name.h"
#include "../component.h"
#include "../observerptr.h"
#include <algorithm>
#include <istream>
#include <ostream>
#include <string>

/*debug*/ bool Name::isEqual(const observer_ptr<const Component> c) const
{
    auto n = static_cast<const Name *>(c);
    return (this->name == n->name);
}

Name *Name::cloneImpl() const { return new Name(*this); }

std::ostream &Name::serialize(std::ostream &os) const
{
    auto word_count =
        std::count(this->name.cbegin(), this->name.cend(), ' ') + 1;
    os << ComponentType::NAME << ' ' << word_count << ' ' << this->name << ' ';
    return os;
}
std::istream &Name::deserialize(std::istream &is)
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

Name::Name(std::string name) { this->name = name; }
Name::Name(const Name &nm) { this->name = nm.name; }

ComponentType Name::getType() const { return ComponentType::NAME; }
