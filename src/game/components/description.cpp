#include "description.h"
#include "../component.h"
#include "../observerptr.h"
#include <algorithm>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

/*debug*/ bool Description::isEqual(const observer_ptr<const Component> c) const
{
    auto d = static_cast<const Description *>(c);
    return (this->description == d->description);
}

Description *Description::cloneImpl() const { return new Description(*this); }

std::ostream &Description::serialize(std::ostream &os) const
{
    auto word_count =
        std::count(this->description.cbegin(), this->description.cend(), ' ') +
        1;
    os << ComponentType::DESCRIPTION << ' ' << word_count << ' '
       << this->description << ' ';
    return os;
}

std::istream &Description::deserialize(std::istream &is)
{
    int         word_count{};
    std::string temp{};
    is >> word_count;
    for (int i = 0; i < word_count - 1; i++)
    {
        is >> temp;
        this->description += temp + " ";
    }
    is >> temp;
    this->description += temp;
    return is;
}

Description::Description(std::string description)
{
    this->description = description;
}
Description::Description(const Description &desc)
{
    this->description = desc.description;
}
ComponentType Description::getType() const
{
    return ComponentType::DESCRIPTION;
}
