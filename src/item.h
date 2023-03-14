#ifndef ITEM_H
#define ITEM_H
#include <string>
#include "rarity.h"

class Item
{
protected:
    std::string _name;
    std::string _description;
    std::string _symbol;
    Rarity _rarity = Rarity::Common;

public:
    // class Builder
    // {
    // protected:
    //     std::string _name{};
    //     std::string _description{};
    //     std::string _symbol{};
    //     Rarity _rarity = Rarity::Common;

    // public:
    //     Builder name(std::string name)
    //     {
    //         _name = name;
    //         return *this;
    //     }
    //     Builder description(std::string description)
    //     {
    //         _description = description;
    //         return *this;
    //     }
    //     Builder symbol(std::string symbol)
    //     {
    //         _symbol = symbol;
    //         return *this;
    //     }
    //     Builder rarity(Rarity rarity)
    //     {
    //         _rarity = rarity;
    //         return *this;
    //     }
    //     virtual Item build()
    //     {
    //         return Item(_name, _description, _symbol, _rarity);
    //     }
    // };

    Item(std::string name, std::string description, std::string symbol, Rarity rarity)
        : _name{name}, _description{description}, _symbol{symbol}, _rarity{rarity}
    {
    }

    Item(Item &&item)
    {
        _name = std::move(item._name);
        _description = std::move(item._description);
        _symbol = std::move(item._symbol);
        _rarity = std::move(item._rarity);
    }

    std::string get_name()
    {
        return _name;
    }
    std::string get_description()
    {
        return _description;
    }
    std::string get_symbol()
    {
        return _symbol;
    }
    Rarity get_rarity()
    {
        return _rarity;
    }
    std::string get_rarity_string()
    {
        switch (_rarity)
        {
        case Rarity::Common:
            return "Common";
        case Rarity::Uncommon:
            return "Uncommon";
        case Rarity::Rare:
            return "Rare";
        case Rarity::Epic:
            return "Epic";
        case Rarity::Legendary:
            return "Legendary";
        }
    }
    std::string get_name_with_prefix()
    {
        return this->get_rarity_string() + " " + _name;
    }
    virtual void on_use()
    {
    }
};

#endif /*ITEM_H*/