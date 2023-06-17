#ifndef GAMESCREEN_H
#define GAMESCREEN_H
#include "../effect.h"
#include "../observerptr.h"
#include "../rarity.h"
#include "../systems/positionsystem.h"
#include "../tile.h"
#include <barrier>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <memory>
#include <ranges>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class GameScreen
{
public: // debug
    class Sprite
    {
        std::string  character;
        ftxui::Color character_color;
        ftxui::Color background_color;

    public:
        Sprite(char         character        = ' ',
               ftxui::Color character_color  = ftxui::Color::Black,
               ftxui::Color background_color = ftxui::Color::White);

        ftxui::Element returnRenderable();
    };

    observer_ptr<std::vector<std::vector<Tile>>> game_map_;
    observer_ptr<Entity>                         player_;
    std::unordered_map<TileAppearance, Sprite>   appearances_;
    PositionSystem                              &pos_system_;
    observer_ptr<const int>                      depth_;

public: // temp for debug
    ftxui::Color getColorByRarity(Rarity rarity) const;

    ftxui::Color getColorByEffect(Effect effect,
                                  bool   is_text_color = false) const;

    std::string getEffectName(Effect effect) const;

    void
    calculateColumn(int starting_index, int index_guard, ftxui::Elements &cols);

    ftxui::Element getMapBox();
    ftxui::Element getSidebar();
    ftxui::Element getEntityHpBar(observer_ptr<Entity>);
    ftxui::Element getPlayerXpBar();
    ftxui::Element getPlayerHungerBar();
    ftxui::Element getPlayerAttackRating();
    ftxui::Element getPlayerArmorRating();
    ftxui::Element getEntityEquipment(observer_ptr<Entity>);
    ftxui::Element getEntityEffects(observer_ptr<Entity>);
    ftxui::Element getLastDamagedEnemy(observer_ptr<Entity>);
    ftxui::Element debugGetMapBox(observer_ptr<Entity>);

public:
    void assignNewMap(observer_ptr<std::vector<std::vector<Tile>>> map);

    void           assignNewPlayer(observer_ptr<Entity> player);
    ftxui::Element render(observer_ptr<Entity>);

    ftxui::Element debugRender(observer_ptr<Entity> enemy);

    GameScreen(observer_ptr<std::vector<std::vector<Tile>>> map,
               observer_ptr<Entity>                         player,
               PositionSystem                              &pos_sys,
               const observer_ptr<const int>                depth);
};

#endif /*GAMESCREEN_H*/
