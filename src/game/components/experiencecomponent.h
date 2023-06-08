#ifndef EXPERIENCECOMPONENT_H
#define EXPERIENCECOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>

class ExperienceComponent : public Component
{
    /*debug*/ bool isEqual(const observer_ptr<const Component> c) const override
    {
        auto e = static_cast<const ExperienceComponent *>(c);
        return (this->level == e->level &&
                this->current_experience == e->current_experience);
    }
    ExperienceComponent *cloneImpl() const override
    {
        return new ExperienceComponent(*this);
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << ComponentType::EXPERIENCE << ' ' << +this->level << ' '
           << this->current_experience << ' ';
        return os;
    }

    std::istream &deserialize(std::istream &is) override
    {
        int level{};
        is >> level >> this->current_experience;
        this->level = static_cast<uint8_t>(level);
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
    ComponentType getType() const override { return ComponentType::EXPERIENCE; }
};

#endif /*EXPERIENCECOMPONENT_H*/
