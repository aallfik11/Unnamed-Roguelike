#ifndef NAME_H
#define NAME_H
#include "../component.h"
#include "../observerptr.h"
#include <istream>
#include <ostream>
#include <string>

class Name : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    Name         *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    std::string name;

    Name(std::string name = "");
    Name(const Name &nm);

    ComponentType getType() const override;
};

#endif /*NAME_H*/
