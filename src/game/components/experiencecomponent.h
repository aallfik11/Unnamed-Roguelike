#ifndef EXPERIENCECOMPONENT_H
#define EXPERIENCECOMPONENT_H
#include "../component.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class ExperienceComponent : public Component
{
    ExperienceComponent *cloneImpl() const override
    {
        return new ExperienceComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::EXPERIENCE << ' ' << +this->level << ' '
           << this->current_experience;
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        is >> this->level >> this->current_experience;
        return is;
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

    ExperienceComponent(const ExperienceComponent &exp)
    {
        this->level              = exp.level;
        this->current_experience = exp.current_experience;
    }
};

#endif /*EXPERIENCECOMPONENT_H*/
