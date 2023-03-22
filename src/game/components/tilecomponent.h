#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H
#include "../tile.h"
#include "../component.h"
#include <string>
#include <ftxui/screen/color.hpp>

class TileComponent : Component
{
public:
    TileType type;
    std::string sprite;
    ftxui::Color color;
    ftxui::Color bg_color;

    TileComponent(TileType type = TileType::SPRITE,
                  std::string sprite = " ",
                  ftxui::Color color = ftxui::Color::White,
                  ftxui::Color bg_color = ftxui::Color::Black)
    {
        this->type = type;
        this->sprite = sprite;
        this->color = color;
        this->bg_color = bg_color;
    }
};

#endif /*TILECOMPONENT_H*/