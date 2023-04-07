#ifndef COMPONENT_H
#define COMPONENT_H

class Component
{
public:
    virtual ~Component() {}
    virtual Component* clone() = 0;
};

#endif /*COMPONENT_H*/