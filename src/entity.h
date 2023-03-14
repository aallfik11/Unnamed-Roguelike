#ifndef ENTITY_H
#define ENTITY_H
#include "item.h"
#include <regex>
#include <memory>
#include <list>

class Entity
{
protected:
    std::string _name;
    std::string _symbol;
    std::list<std::unique_ptr<Item>> _inventory;
    int _pos_x, _pos_y;

public:
    Entity(std::string name, int pos_x, int pos_y, std::list<std::unique_ptr<Item>> *inventory = nullptr)
        : _name{name}, _pos_x{pos_x}, _pos_y{pos_y}
    {
        if (inventory != nullptr)
            _inventory = std::move(*inventory);
    }

    std::string get_name()
    {
        return _name;
    }

    virtual std::list<std::unique_ptr<Item>> &get_inventory()
    {
        return _inventory;
    }

    void add_to_inventory(std::unique_ptr<Item> item)
    {
        _inventory.push_back(std::move(item));
    }

    std::unique_ptr<Item> pull_item_out(int index)
    {
        auto it = _inventory.begin();
        while (index < 0)
        {
            it++;
            index--;
        }
        auto item = std::move(*it);
        _inventory.erase(it);
        return item;
    }

    int get_pos_x()
    {
        return _pos_x;
    }

    void set_pos_x(int _pos_x)
    {
        this->_pos_x = _pos_x;
    }

    int get_pos_y()
    {
        return _pos_y;
    }

    void set_pos_y(int _pos_y)
    {
        this->_pos_y = _pos_y;
    }

    std::string get_symbol()
    {
        return _symbol;
    }

    virtual void on_interact(const Entity *actor)
    {
    }
};

#endif /*ENTITY_H*/