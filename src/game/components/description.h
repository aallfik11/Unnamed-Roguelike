#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include <algorithm>
#include <istream>
#include <memory>
#include <ostream>
#include <string>

class Description : public Component
{
    /*debug*/ bool isEqual(const Component *const c) const override
    {
        auto d = static_cast<const Description *>(c);
        return (this->description == d->description);
    }

    Description  *cloneImpl() const override { return new Description(*this); }
    std::ostream &serialize(std::ostream &os) const override
    {
        auto word_count = std::count(this->description.cbegin(),
                                     this->description.cend(),
                                     ' ') +
                          1;
        os << ComponentType::DESCRIPTION << ' ' << word_count << ' '
           << this->description << ' ';
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
            this->description += temp + " ";
        }
        is >> temp;
        this->description += temp;
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
