#ifndef STACKABLE_H
#define STACKABLE_H
#include "../component.h"
#include <cstdint>

class Stackable : Component
{

public:
    uint32_t max_stack_count;
    uint32_t current_stack_count;

    Stackable(uint32_t max_stack_count = 1, uint32_t current_stack_count = 0)
    {
        this->max_stack_count = max_stack_count;
        this->current_stack_count = current_stack_count;
    }
};

#endif /*STACKABLE_H*/