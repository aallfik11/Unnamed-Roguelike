#include "health.h"
#include "../component.h"
#include "../observerptr.h"
#include <cstdint>
#include <istream>
#include <ostream>

/*debug*/ bool Health::isEqual(const observer_ptr<const Component> c) const
{
    auto h = static_cast<const Health *>(c);
    return (this->max_health_points == h->max_health_points &&
            this->current_health_points == h->current_health_points &&
            this->alive == h->alive);
}

Health       *Health::cloneImpl() const { return new Health(*this); }
std::ostream &Health::serialize(std::ostream &os) const
{
    os << ComponentType::HEALTH << ' ' << this->max_health_points << ' '
       << this->current_health_points << ' ' << this->alive << ' ';
    return os;
}

std::istream &Health::deserialize(std::istream &is)
{
    is >> this->max_health_points >> this->current_health_points >> this->alive;
    return is;
}

Health::Health()
{
    max_health_points     = 0;
    current_health_points = 0;
    alive                 = true;
}

Health::Health(uint16_t max_hp)
{
    max_health_points     = max_hp;
    current_health_points = max_hp;
    alive                 = true;
}

Health::Health(uint16_t max_hp, uint16_t current_hp, bool alive)
{
    max_health_points     = max_hp;
    current_health_points = (current_hp > max_hp) ? max_hp : current_hp;
    this->alive           = alive;
}

Health::Health(const Health &hp)
{
    this->max_health_points     = hp.max_health_points;
    this->current_health_points = hp.current_health_points;
    this->alive                 = hp.alive;
}
ComponentType Health::getType() const { return ComponentType::HEALTH; }
