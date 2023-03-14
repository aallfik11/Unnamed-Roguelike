#ifndef ENTITY_H
#define ENTITY_H
#include <ftxui/screen/color.hpp>
#include <regex>
#include <memory>
#include <list>
#include "item.h"

class Entity
{
protected:
    std::string _name;
    std::string _symbol;
    ftxui::Color _color;
    std::list<std::unique_ptr<Item>> _inventory;
    int _pos_x, _pos_y;

public:
    Entity(std::string name, std::string symbol, ftxui::Color color, int pos_x, int pos_y, std::list<std::unique_ptr<Item>> *inventory = nullptr)
        : _name{name}, _symbol{symbol}, _color{color} ,_pos_x{pos_x}, _pos_y{pos_y}
    {
        if (inventory != nullptr)
            _inventory = std::move(*inventory);
    }

    /**
     * @brief Copy constructor, perhaps I'll never use it in which case I'll delete it
     *
     * @param entity
     */
    Entity(Entity &entity)
    {
        this->_name = entity._name;
        this->_symbol = entity._symbol;
        this->_pos_x = entity._pos_x;
        this->_pos_y = entity._pos_y;

        // now for the fun part

        if (!entity._inventory.empty())
        {
            this->_inventory.clear();
            std::list<std::unique_ptr<Item>> temp_inv;

            // this looks terrible, maybe if I weren't afraid of manual memory management I could make it better :(
            for (auto &it : entity._inventory)
            {
                Item *itemptr = it.release();
                Item item = *itemptr;
                delete itemptr;
                std::unique_ptr<Item> item1(new Item(item));
                std::unique_ptr<Item> item2(new Item(item));
                temp_inv.push_back(std::move(item1));
                this->_inventory.push_back(std::move(item2));
            }
            entity._inventory = std::move(temp_inv);
        }
    }

    /**
     * @brief Move constructor
     *
     * @param entity
     */
    Entity(Entity &&entity)
    {
        this->_name = std::move(entity._name);
        this->_symbol = std::move(entity._symbol);
        this->_inventory = std::move(entity._inventory);
        this->_pos_x = std::move(entity._pos_x);
        this->_pos_y = std::move(entity._pos_y);
    }

    std::string get_name()
    {
        return _name;
    }

    std::string get_symbol()
    {
        return _symbol;
    }

    virtual std::list<std::unique_ptr<Item>> &get_inventory()
    {
        return _inventory;
    }

    void add_to_inventory(std::unique_ptr<Item> &item)
    {
        _inventory.push_back(std::move(item));
    }

    void add_to_inventory(Item *item = nullptr)
    {
        if (item != nullptr)
        {
            std::unique_ptr<Item> temp(new Item(std::move(*item)));
            _inventory.push_back(std::move(temp));
        }
    }

    // since all items are going to be created with smart pointers, extra care needs to go into making sure I don't leave this pointer dangling

    /**
     * @brief Pulls out an item at the specified index of the inventory
     *
     * @param index
     * @return Item* null if invalid index
     */
    Item *pull_item_out(int index = 0)
    {
        if (index >= _inventory.size() || index < 0)
            return nullptr;

        auto it = _inventory.begin();
        while (index < 0)
        {
            it++;
            index--;
        }
        auto item = std::move(*it);
        _inventory.erase(it);
        return item.release();
    }

    void transfer_inventory(Entity *target)
    {
        auto &target_list = target->get_inventory();
        target_list.splice(target_list.end(), this->_inventory);
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

    virtual void on_interact(const Entity *actor)
    {
    }

    virtual void on_tick()
    {
    }
};

#endif /*ENTITY_H*/