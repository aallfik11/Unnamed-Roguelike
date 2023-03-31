#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H
#include "../component.h"
#include "../tile.h"
#include <ftxui/screen/color.hpp>
#include <string>

class TileComponent : public Component
{
public:
    Tile        tile;
    std::string sprite;

    TileComponent(TileType     type     = TileType::SPRITE,
                  std::string  sprite   = " ",
                  ftxui::Color color    = ftxui::Color::White,
                  ftxui::Color bg_color = ftxui::Color::Black)
    {
        this->tile.type             = type;
        this->sprite                = sprite;
        this->tile.color            = color;
        this->tile.background_color = bg_color;
    }
};

#endif /*TILECOMPONENT_H*/