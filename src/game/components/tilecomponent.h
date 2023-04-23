#ifndef TILECOMPONENT_H
#define TILECOMPONENT_H
#include "../component.h"
#include "../tile.h"
#include <ftxui/screen/color.hpp>
#include <memory>
#include <string>

class TileComponent : public Component
{

    TileComponent *cloneImpl() const override
    {
        return new TileComponent(*this);
    }

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

    TileComponent(const TileComponent &tile_component)
    {
        this->tile.type             = tile_component.tile.type;
        this->tile.color            = tile_component.tile.color;
        this->tile.background_color = tile_component.tile.background_color;
        this->sprite                = tile_component.sprite;
    }

    std::unique_ptr<TileComponent> clone() const
    {
        return std::unique_ptr<TileComponent>(this->cloneImpl());
    }
};

#endif /*TILECOMPONENT_H*/