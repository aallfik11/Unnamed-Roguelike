#ifndef EXPERIENCECOMPONENT_H
#define EXPERIENCECOMPONENT_H
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

class ExperienceComponent : public Component
{
    /*debug*/ bool
    isEqual(const observer_ptr<const Component> c) const override;

    ExperienceComponent *cloneImpl() const override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

public:
    uint8_t  level;
    uint32_t current_experience; // exp require for level up will be defined by
                                 // this formula: target level * 25;

    ExperienceComponent(uint8_t level = 1, uint32_t current_experience = 0);
    ExperienceComponent(const ExperienceComponent &exp);

    ComponentType getType() const override;
};

#endif /*EXPERIENCECOMPONENT_H*/
