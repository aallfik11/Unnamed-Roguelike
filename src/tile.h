#ifndef TILE_H
#define TILE_H
#include "entity.h"
#include "item.h"
#include "TileTypes.h"
#include <list>
#include <ftxui/screen/color.hpp>

class Tile
{
    TileType _type;
    ftxui::Color _color;
    bool _gen_flag;
    std::list<std::unique_ptr<Item>> _items;
    std::list<Entity *> _entities;

public:
    Tile() {}

    Tile(TileType type = FLOOR, ftxui::Color color = ftxui::Color::White)
        : _type{type}
        , _color{color}
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

    
};

#endif /*TILE_H*/