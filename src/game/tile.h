#ifndef TILE_H
#define TILE_H
#include <cstdint>
#include <ftxui/screen/color.hpp>

enum TileType : uint16_t
{
    FLOOR       = 0x0001,
    WALL        = 0x0002,
    SPRITE      = 0x0004, // might turn out to be unnecessary
    TRAVERSIBLE = 0x0008,
    CHECKED     = 0x0010, // used in map generation

    HAS_CREATURE = 0x0020,
    HAS_ITEM    = 0x0040,
    HAS_STAIRS  = 0x0080,

    MAPPED      = 0x0100,
    VISIBLE     = 0x0200,
};

inline constexpr TileType operator&(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint16_t>(type_1) &
                                 static_cast<uint16_t>(type_2));
}

inline constexpr TileType operator|(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint16_t>(type_1) |
                                 static_cast<uint16_t>(type_2));
}

inline constexpr TileType operator^(TileType type_1, TileType type_2)
{
    return static_cast<TileType>(static_cast<uint16_t>(type_1) ^
                                 static_cast<uint16_t>(type_2));
}

inline constexpr TileType operator~(TileType type_1)
{
    return static_cast<TileType>(~static_cast<uint16_t>(type_1));
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