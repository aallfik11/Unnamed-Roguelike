#ifndef TILE_H
#define TILE_H
#include <cstdint>
#include <ftxui/screen/color.hpp>

enum TileType : uint8_t
{
    FLOOR       = 0x01,
    WALL        = 0x02,
    SPRITE      = 0x04,
    TRAVERSIBLE = 0x08,
    CHECKED     = 0x10
};

inline constexpr TileType operator&(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint8_t>(type_1) &
                                 static_cast<uint8_t>(type_2));
}

inline constexpr TileType operator|(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint8_t>(type_1) |
                                 static_cast<uint8_t>(type_2));
}

inline constexpr TileType operator^(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint8_t>(type_1) ^
                                 static_cast<uint8_t>(type_2));
}

inline constexpr TileType operator~(TileType type_1)
{
    return static_cast<TileType>(~static_cast<uint8_t>(type_1));
}

inline TileType &operator&=(TileType &type_1, TileType type_2)
{
    type_1 = type_1 & type_2;
    return type_1;
}

inline TileType &operator|=(TileType &type_1, TileType type_2)
{
    type_1 = type_1 | type_2;
    return type_1;
}

inline TileType &operator^=(TileType &type_1, TileType type_2)
{
    type_1 = type_1 ^ type_2;
    return type_1;
}

struct Tile
{
    TileType     type             = TileType(FLOOR | TRAVERSIBLE);
    ftxui::Color color            = ftxui::Color::White;
    ftxui::Color background_color = ftxui::Color::Black;
};

#endif /*TILE_H*/