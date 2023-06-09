#ifndef TILE_H
#define TILE_H
#include <cstdint>
#include <istream>
#include <ostream>

enum class TileType : uint16_t
{
    NONE         = 0x0000,
    FLOOR        = 0x0001,
    WALL         = 0x0002,
    SPRITE       = 0x0004, // might turn out to be unnecessary
    TRAVERSIBLE  = 0x0008,
    CHECKED      = 0x0010, // used in map generation

    HAS_CREATURE = 0x0020,
    HAS_ITEM     = 0x0040,
    HAS_STAIRS   = 0x0080,

    MAPPED       = 0x0100, // used in line of sight calcualtion
    VISIBLE      = 0x0200, // used in line of sight calculation
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
inline std::ostream &operator<<(std::ostream &os, const TileType &type)
{
    os << static_cast<uint16_t>(type);
    return os;
}
inline std::istream &operator>>(std::istream &is, TileType &type)
{
    uint16_t temp{};
    is >> temp;
    type = static_cast<TileType>(temp);
    return is;
}

enum class TileAppearance : uint16_t
{
    //can be used for invisibility later on
    NONE,

    // MAP TILES
    WALL,
    FLOOR,
    STAIRS,
    MAPPED_WALL,
    MAPPED_FLOOR,
    MAPPED_STAIRS,
    UNMAPPED,

    // ITEMS
    WEAPON,
    ARMOR,
    RING,
    POTION,
    FOOD,
    GOAL_ITEM,

    // CREATURES
    PLAYER,
    // ANIMALS
    RAT,
    BAT,
    GIANT_SPIDER,
    VIPER,

    // HUMANOID
    KOBOLD,
    GOBLIN,
    ORC,
    TROLL,
    ZOMBIE,
    DEATHKNIGHT,
    ASSASSIN,
    DEMON,
    VAMPIRE, // NOT SURE

    // OTHER
    DRAGON,
    GIANT_VENUS_FLYTRAP,

};
inline std::ostream &operator<<(std::ostream         &os,
                                const TileAppearance &appearance)
{
    os << static_cast<uint16_t>(appearance);
    return os;
}
inline std::istream &operator>>(std::istream &is, TileAppearance &appearance)
{
    uint16_t temp{};
    is >> temp;
    appearance = static_cast<TileAppearance>(temp);
    return is;
}

struct Tile
{
    TileType                    type = TileType::FLOOR | TileType::TRAVERSIBLE;
    TileAppearance              appearance = TileAppearance::FLOOR;
    // TileColor -- make an enum for that
    //  ftxui::Color color;
    //  ftxui::Color background_color;
    inline friend std::ostream &operator<<(std::ostream &os, const Tile &tile)
    {
        os << tile.type << ' ' << tile.appearance;
        return os;
    }

    inline friend std::istream &operator>>(std::istream &is, Tile &tile)
    {
        is >> tile.type >> tile.appearance;
        return is;
    }
};

#endif /*TILE_H*/
