#ifndef LINEOFSIGHTCOMPONENT_H
#define LINEOFSIGHTCOMPONENT_H
#include "../component.h"

class LOSComponent : public Component
{
public:
    // I want to make it work for other entities later on
    bool hasLOSToPlayer = false;
};

#endif /*LINEOFSIGHTCOMPONENT_H*/