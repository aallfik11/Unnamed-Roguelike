#ifndef DESCRIPTION_H
#define DESCRIPTION_H
#include "../component.h"
#include "../observerptr.h"
#include <istream>
#include <ostream>
#include <string>

class Description : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    Description *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    std::string description;

    Description(std::string description = "");
    Description(const Description &desc);

    ComponentType getType() const override;
};

#endif /*DESCRIPTION_H*/
