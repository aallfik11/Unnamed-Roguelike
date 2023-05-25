#ifndef GAMESCREEN_H
#define GAMESCREEN_H
#include "../components/amuletslot.h"
#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/health.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../observerptr.h"
#include "../systems/mapmanager.h"
#include "../tile.h"
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class GameScreen
{
    observer_ptr<std::vector<std::vector<Tile>>>       game_map_;
    std::unordered_map<TileAppearance, ftxui::Element> appearances_;

    ftxui::Element getMapBox() {
        using namespace ftxui;
        Elements columns;
        for(std::size_t x = 0; x < game_map_->size(); ++x)
        {
            Elements cells;
            for(std::size_t y = 0; y < game_map_[x].size(); ++y)
            {
            }
        }

    }
    ftxui::Color   floor_color;

public:
    GameScreen(observer_ptr<std::vector<std::vector<Tile>>> map) : GameScreen()
    {
        game_map_ = map;
    }
    void assignNewMap(observer_ptr<std::vector<std::vector<Tile>>> map)
    {
        game_map_ = map;
    }
    void render() {}

    GameScreen()
    {
        using namespace ftxui;
        auto wall_color        = Color::RGB(176, 176, 176);
        floor_color            = Color::RGB(88, 88, 88);
        auto stairs_color      = Color::RGB(22, 22, 22);
        auto weapon_color      = Color::RGB(240, 240, 240);
        auto armor_color       = Color::RGB(240, 240, 240);
        auto ring_color        = Color::RGB(255, 255, 0);
        auto potion_color      = Color::RGB(255, 50, 80);
        auto food_color        = Color::RGB(160, 80, 40);
        auto goal_item_color   = Color::RGB(255, 40, 255);
        auto player_color      = Color::RGB(255, 255, 255);
        auto rat_color         = Color::RGB(140, 140, 140);
        auto bat_color         = Color::RGB(140, 70, 35);
        auto spider_color      = Color::RGB(153, 0, 0);
        auto viper_color       = Color::RGB(50, 180, 80);
        auto kobold_color      = Color::RGB(204, 0, 0);
        auto goblin_color      = Color::RGB(80, 150, 10);
        auto orc_color         = Color::RGB(50, 150, 10);
        auto troll_color       = Color::RGB(120, 130, 150);
        auto zombie_color      = Color::RGB(140, 170, 140);
        auto deathknight_color = Color::RGB(128, 128, 128);
        auto assassin_color =
            Color::RGB(100, 100, 100); // intentionally hard to see
        auto demon_color                     = Color::RGB(140, 0, 70);
        auto vampire_color                   = Color::RGB(230, 230, 230);
        auto dragon_color                    = Color::RGB(51, 51, 255);
        auto venus_flytrap_color             = Color::RGB(0, 204, 0);

        // appearances_[TileAppearance::NONE] =
        //     text("") | color(Color::RGB()) | bgcolor(Color::RGB());
        appearances_[TileAppearance::WALL]   = text(" ") | bgcolor(wall_color);
        appearances_[TileAppearance::FLOOR]  = text(" ") | bgcolor(floor_color);
        appearances_[TileAppearance::STAIRS] = text("#") | color(stairs_color);
        appearances_[TileAppearance::WEAPON] = text("/") | color(weapon_color);
        appearances_[TileAppearance::ARMOR]  = text("%") | color(armor_color);
        appearances_[TileAppearance::RING]   = text("?") | color(ring_color);
        appearances_[TileAppearance::POTION] = text("!") | color(potion_color);
        appearances_[TileAppearance::FOOD]   = text("*") | color(food_color);
        appearances_[TileAppearance::GOAL_ITEM] =
            text("$") | color(goal_item_color);
        appearances_[TileAppearance::PLAYER] = text("@") | color(player_color);
        appearances_[TileAppearance::RAT]    = text("R") | color(rat_color);
        appearances_[TileAppearance::BAT]    = text("B") | color(bat_color);
        appearances_[TileAppearance::GIANT_SPIDER] =
            text("S") | color(spider_color);
        appearances_[TileAppearance::VIPER]  = text("V") | color(viper_color);
        appearances_[TileAppearance::KOBOLD] = text("K") | color(kobold_color);
        appearances_[TileAppearance::GOBLIN] = text("G") | color(goblin_color);
        appearances_[TileAppearance::ORC]    = text("O") | color(orc_color);
        appearances_[TileAppearance::TROLL]  = text("T") | color(troll_color);
        appearances_[TileAppearance::ZOMBIE] = text("Z") | color(zombie_color);
        appearances_[TileAppearance::DEATHKNIGHT] =
            text("D") | color(deathknight_color);
        appearances_[TileAppearance::ASSASSIN] =
            text("A") | color(assassin_color);
        appearances_[TileAppearance::DEMON] = text("D") | color(demon_color);
        appearances_[TileAppearance::VAMPIRE] =
            text("V") | color(vampire_color);
        appearances_[TileAppearance::DRAGON] = text("D") | color(dragon_color);
        appearances_[TileAppearance::GIANT_VENUS_FLYTRAP] =
            text("F") | color(venus_flytrap_color);
        for (auto &[key, sprite] : appearances_)
        {
            if (key == TileAppearance::FLOOR || key == TileAppearance::WALL)
                continue;
            sprite |= bgcolor(floor_color);
        }
    }
};

#endif /*GAMESCREEN_H*/
