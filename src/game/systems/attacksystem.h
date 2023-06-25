#ifndef ATTACKSYSTEM_H
#define ATTACKSYSTEM_H
#include "../entity.h"
#include "../system.h"
#include <cstdint>
#include <istream>
#include <list>
#include <ostream>
#include <random>
#include <ranges>

class AttackSystem : public System
{

    std::random_device              rd_;
    std::mt19937                    mt_engine_;
    std::uniform_int_distribution<> roll_chance_;
    std::list<std::pair<observer_ptr<Entity>, observer_ptr<Entity>>> messages_;

    uint8_t truncateArmorClass(uint8_t ac);

public:
    AttackSystem();

    bool attack(const observer_ptr<Entity> attacker,
                const observer_ptr<Entity> defender);

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
};

#endif /*ATTACKSYSTEM_H*/
