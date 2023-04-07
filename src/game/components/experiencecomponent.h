#ifndef EXPERIENCECOMPONENT_H
#define EXPERIENCECOMPONENT_H
#include "../component.h"
#include <cstdint>

class ExperienceComponent : public Component
{
public:
    uint8_t  level;
    uint32_t current_experience; // exp require for level up will be defined by
                                 // this formula: target level * 25;

    ExperienceComponent(uint8_t level, uint32_t current_experience)
    {
        this->level              = level;
        this->current_experience = current_experience;
    }

    ExperienceComponent *clone()
    {
        return new ExperienceComponent(this->level, this->current_experience);
    }
};

#endif /*EXPERIENCECOMPONENT_H*/