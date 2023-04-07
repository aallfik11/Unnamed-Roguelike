#ifndef TYPECOMPONENT_H
#define TYPECOMPONENT_H
#include "../component.h"
#include "../entitytypes.h"

class Type : public Component // unused rn
{
public:
    EntityType type;
    Type(EntityType type) { this->type = type; }
};

#endif /*TYPECOMPONENT_H*/