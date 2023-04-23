#ifndef EXPERIENCECOMPONENT_H
#define EXPERIENCECOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <memory>


class ExperienceComponent : public Component
{

    ExperienceComponent *cloneImpl() const override
    {
        return new ExperienceComponent(this->level, this->current_experience);
    }

public:
    uint8_t  level;
    uint32_t current_experience; // exp require for level up will be defined by
                                 // this formula: target level * 25;

    ExperienceComponent(uint8_t level = 1, uint32_t current_experience = 0)
    {
        this->level              = level;
        this->current_experience = current_experience;
    }

    std::unique_ptr<ExperienceComponent> clone() const
    {
        return std::unique_ptr<ExperienceComponent>(this->cloneImpl());
    }
};

#endif /*EXPERIENCECOMPONENT_H*/