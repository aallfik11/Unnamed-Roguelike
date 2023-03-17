#ifndef CONSUMABLE_H
#define CONSUMABLE_H
#include "item.h"
#include "entity.h"

class Consumable : Item
{
    int _charges;

public:
    // class Builder : Item::Builder
    // {
    // protected:
    //     int _charges = 1;
    //     Builder charges(int charges)
    //     {
    //         _charges = charges;
    //         return *this;
    //     }
    //     Item build() override
    //     {
    //         return Consumable(Builder::Builder::build(), _charges);
    //     }
    // };

    Consumable(std::string name, std::string description, std::string symbol, Rarity rarity, int uses)
        : Item(name, description, symbol, rarity)
    {
    }
    Consumable(Item item, int charges) : Item(std::move(item)), _charges{charges}
    {}

    ~Consumable()
    {
        // on_use();
    }
   
};

#endif /*CONSUMABLE_H*/