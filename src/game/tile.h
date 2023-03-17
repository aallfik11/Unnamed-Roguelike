#ifndef TILE_H
#define TILE_H
#include "entity.h"
#include "item.h"
#include "TileTypes.h"
#include <list>
#include <memory>
#include <ftxui/screen/color.hpp>

class Tile
{
    TileType _type;
    ftxui::Color _color;
    std::list<std::unique_ptr<Item>> _items;
    std::weak_ptr<Entity> _entity;

public:
    Tile() {}

    Tile(TileType type = FLOOR, ftxui::Color color = ftxui::Color::White)
        : _type{type}, _color{color}
    {
    }

    void set_type(TileType type)
    {
        this->_type = type;
    }
    TileType get_type()
    {
        return _type;
    }

    void set_color(ftxui::Color color)
    {
        this->_color = color;
    }

    ftxui::Color get_color()
    {
        return _color;
    }

    std::list<std::unique_ptr<Item>> &get_items()
    {
        return _items;
    }

    void add_item(std::unique_ptr<Item>& item)
    {
        _items.push_back(std::move(item));
    }

    void add_item(Item* item = nullptr)
    {
        if (item != nullptr)
        {
            std::unique_ptr<Item> temp(new Item(std::move(*item)));
            _items.push_back(std::move(temp));
        }
    }

    //considering I've got the same method in entity.h, maybe I shouldve structured the code a little better

    Item *pull_item_out(int index = 0)
    {
        if (index >= _items.size() || index < 0)
            return nullptr;

        auto it = _items.begin();
        while (index < 0)
        {
            it++;
            index--;
        }
        auto item = std::move(*it);
        _items.erase(it);
        return item.release();
    }

    void transfer_items(Entity *target)
    {
        auto &target_list = target->get_inventory();
        target_list.splice(target_list.end(), this->_items);
    }

    void add_entity(std::shared_ptr<Entity> &entity)
    {
        this->_entity = entity;
    }



};

#endif /*TILE_H*/