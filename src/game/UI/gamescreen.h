#ifndef GAMESCREEN_H
#define GAMESCREEN_H
#include "../components/amuletslot.h"
#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/experiencecomponent.h"
#include "../components/health.h"
#include "../components/hungercomponent.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/tilecomponent.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../entity.h"
#include "../globals.h"
#include "../observerptr.h"
#include "../rarity.h"
#include "../systems/mapmanager.h"
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
               ftxui::Color background_color = ftxui::Color::White)
        {
            this->character        = character;
            this->character_color  = character_color;
            this->background_color = background_color;
        }
        ftxui::Element returnRenderable()
        {
            using namespace ftxui;
            return text(character) | color(character_color) |
                   bgcolor(background_color);
        }
    };

    observer_ptr<std::vector<std::vector<Tile>>> game_map_;
    observer_ptr<Entity>                         player_;
    std::unordered_map<TileAppearance, Sprite>   appearances_;
    PositionSystem                              &pos_system_;
    ftxui::Color                                 floor_color;

    inline ftxui::Color getColorByRarity(Rarity rarity) const
    {
        using namespace ftxui;

        switch (rarity)
        {
        case Rarity::COMMON:
            return ftxui::Color::GrayLight;
        case Rarity::UNCOMMON:
            return ftxui::Color::Green3;
        case Rarity::RARE:
            return ftxui::Color::Cyan;
        case Rarity::EPIC:
            return ftxui::Color::Purple;
        case Rarity::LEGENDARY:
            return ftxui::Color::Gold1;
        default:
            return ftxui::Color::White;
        }
    }

    inline ftxui::Color getColorByEffect(Effect effect) const
    {
        using namespace ftxui;
        effect &= ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
        switch (effect)
        {
        case Effect::HEAL:
            return Color::Pink1;
        case Effect::BLEED:
            return Color::Red;
        case Effect::BLIND:
            return Color::GrayLight;
        case Effect::POISON:
            return Color::Green;
        case Effect::IRONSKIN:
            return Color::Yellow;
        case Effect::STRENGTH:
            return Color::DarkRed;
        default:
            return Color::White;
        }
    }

    inline std::string getEffectName(Effect effect) const
    {
        auto effect_cpy = effect;
        effect_cpy &=
            ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
        std::string name = "";
        switch (effect_cpy)
        {
        case Effect::HEAL:
            name = "HEAL";
            break;
        case Effect::BLEED:
            name = "BLEED";
            break;
        case Effect::BLIND:
            name = "BLIND";
            break;
        case Effect::POISON:
            name = "POISON";
            break;
        case Effect::IRONSKIN:
            name = "IRONSKIN";
            break;
        case Effect::STRENGTH:
            name = "STRENGTH";
            break;
        default:
            name = "INVALID";
        }
        if ((effect & Effect::PERMANENT) != Effect::NONE)
        {
            name += " PERM";
        }
        return name;
    }

    inline void
    calculateColumn(int starting_index, int index_guard, ftxui::Elements &cols)
    {
        auto find_creature = [](const observer_ptr<Entity> entity)
        {
            if ((entity->type & (EntityType::PLAYER | EntityType::CREATURE)) !=
                EntityType::NONE)
            {
                return true;
            }
            return false;
        };
        auto find_item = [](const observer_ptr<Entity> entity)
        {
            if ((entity->type & EntityType::ITEM) != EntityType::NONE)
                return true;
            return false;
        };
        using namespace ftxui;
        for (int x = starting_index; x < index_guard; ++x)
        {
            Elements cells(G_MAP_HEIGHT);
            for (std::size_t y = 0; y < G_MAP_HEIGHT; ++y)
            {
                auto entities_at_coordinates =
                    pos_system_.getEntitiesAtCoordinates(x, y);
                if (((*game_map_)[x][y].type & TileType::HAS_CREATURE) !=
                    TileType::NONE)
                {
                    if (auto creature = std::ranges::find_if(
                            entities_at_coordinates.begin(),
                            entities_at_coordinates.end(),
                            find_creature);
                        creature != entities_at_coordinates.end())
                    {
                        auto creature_ptr = *creature;
                        cells[y] =
                            appearances_[creature_ptr
                                             ->getComponent<TileComponent>()
                                             ->sprite]
                                .returnRenderable();
                    }
                }
                else if (((*game_map_)[x][y].type & TileType::HAS_ITEM) !=
                         TileType::NONE)
                {
                    if (auto item = std::ranges::find_if(
                            entities_at_coordinates.begin(),
                            entities_at_coordinates.end(),
                            find_item);
                        item != entities_at_coordinates.end())
                    {
                        auto item_ptr = *item;
                        cells[y] =
                            appearances_[item_ptr->getComponent<TileComponent>()
                                             ->sprite]
                                .returnRenderable();
                    }
                }
                else if (((*game_map_)[x][y].type & TileType::HAS_STAIRS) !=
                         TileType::NONE) [[unlikely]]
                {
                    cells[y] =
                        appearances_[TileAppearance::STAIRS].returnRenderable();
                }
                else
                {
                    cells[y] = appearances_[(*game_map_)[x][y].appearance]
                                   .returnRenderable();
                }
            }
            cols[x] = vbox(cells);
        }
    }

    ftxui::Element getMapBox()
    {

        using namespace ftxui;
        Elements     columns(G_MAP_WIDTH);
        std::barrier sync(11);
        auto         work = [&](int starting_index, int index_guard)
        {
            calculateColumn(starting_index, index_guard, columns);
            sync.arrive_and_wait();
        };

        std::list<std::jthread> workers;
        for (int i = 0; i < 10; ++i)
        {
            int index = i * 10;
            workers.emplace_back(std::jthread(work, index, index + 10));
        }
        sync.arrive_and_wait();

        return hbox(columns);
    }
    ftxui::Element getSidebar()
    {
        using namespace ftxui;
        return vbox({getPlayerHpBar(),
                     separatorEmpty(),
                     getPlayerHungerBar(),
                     separatorEmpty(),
                     getPlayerXpBar(),
                     separatorEmpty(),
                     getPlayerAttackRating(),
                     separatorEmpty(),
                     getPlayerArmorRating(),
                     separatorEmpty(),
                     getPlayerEquipment(),
                     separatorEmpty(),
                     getPlayerEffects()});
    }

    inline ftxui::Element getPlayerHpBar()
    {
        using namespace ftxui;
        auto player_hp  = player_->getComponent<Health>();
        auto hp_percent = player_hp->current_health_points /
                          static_cast<float>(player_hp->max_health_points);

        return vbox(
            {text(std::to_string(player_hp->current_health_points) + " / " +
                  std::to_string(player_hp->max_health_points)) |
                 hcenter,
             gaugeRight(hp_percent) | color(Color::RGB(255, 0, 0)) |
                 borderRounded});
    }

    inline ftxui::Element getPlayerXpBar()
    {
        using namespace ftxui;
        auto player_xp   = player_->getComponent<ExperienceComponent>();
        auto adjusted_xp = player_xp->current_experience;
        for (int i = 2; i <= player_xp->level;
             i++) // i starts at 2 since player starts at lvl 1 and 25
                  // experience for level 1 should not be deduced
        {
            adjusted_xp -= i * 25;
        }
        auto xp_percentage =
            adjusted_xp / static_cast<float>((player_xp->level + 1) * 25);
        return vbox(
            {text(std::to_string(adjusted_xp) + " / " +
                  std::to_string((player_xp->level + 1) * 25)),
             hbox({text(std::to_string(player_xp->level)) | borderRounded,
                   gaugeRight(xp_percentage) | color(Color::YellowLight) |
                       borderRounded,
                   text(std::to_string(player_xp->level + 1)) |
                       borderRounded})});
    }

    inline ftxui::Element getPlayerHungerBar()
    {
        using namespace ftxui;
        auto player_hunger  = player_->getComponent<HungerComponent>();
        auto hunger_percent = player_hunger->hunger /
                              static_cast<float>(player_hunger->max_hunger);

        return vbox(
            {text("Nutrition: " + std::to_string(player_hunger->hunger) +
                  " / " + std::to_string(player_hunger->max_hunger)),
             gaugeRight(hunger_percent) | color(Color::RGB(155, 103, 60)) |
                 borderRounded});
    }

    inline ftxui::Element getPlayerAttackRating()
    {
        using namespace ftxui;
        std::string attack_damage{};
        if (auto player_weapon =
                player_->getComponent<WeaponSlot>()->weapon_item)
        {
            attack_damage = std::to_string(
                player_weapon->getComponent<WeaponComponent>()->damage);
        }
        else
            attack_damage = std::to_string(
                player_->getComponent<WeaponComponent>()->damage);
        return vbox(text("Attack Damage"), text(attack_damage)) | borderRounded;
    }

    inline ftxui::Element getPlayerArmorRating()
    {

        using namespace ftxui;
        std::string armor_rating{};
        if (auto player_armor = player_->getComponent<ArmorSlot>()->armor_item)
        {
            armor_rating = std::to_string(
                player_armor->getComponent<ArmorComponent>()->armor_class);
        }
        else
            armor_rating = std::to_string(
                player_->getComponent<ArmorComponent>()->armor_class);
        return vbox(text("Armor Class"), text(armor_rating)) | borderRounded;
    }

    inline ftxui::Element getPlayerEquipment()
    {
        using namespace ftxui;
        Elements    equipment;
        std::string item_name{};
        Rarity      item_rarity{};
        if (auto weapon = player_->getComponent<WeaponSlot>()->weapon_item)
        {
            item_name   = weapon->getConstComponent<Name>()->name;
            item_rarity = weapon->getConstComponent<ItemComponent>()->rarity;
            equipment.push_back(text(item_name) | hcenter |
                                color(getColorByRarity(item_rarity)));
        }
        if (auto armor = player_->getComponent<ArmorSlot>()->armor_item)
        {
            item_name   = armor->getConstComponent<Name>()->name;
            item_rarity = armor->getConstComponent<ItemComponent>()->rarity;
            equipment.push_back(text(item_name) | hcenter |
                                color(getColorByRarity(item_rarity)));
        }
        for (auto &ring : player_->getComponent<AmuletSlot>()->amulet_slots)
        {
            item_name   = ring->getConstComponent<Name>()->name;
            item_rarity = ring->getConstComponent<ItemComponent>()->rarity;
            equipment.push_back(text(item_name) | hcenter |
                                color(getColorByRarity(item_rarity)));
        }
        return vbox(equipment) | borderRounded;
    }

    inline ftxui::Element getPlayerEffects()
    {
        using namespace ftxui;
        Elements buffs;
        for (auto &buff : player_->getConstComponent<BuffComponent>()->buffs)
        {
            buffs.push_back(text(getEffectName(buff.first)) | hcenter |
                            bgcolor(getColorByEffect(buff.first)));
        }
        return vbox(buffs) | borderRounded;
    }

    inline ftxui::Element getLastDamagedEnemy()
    {
        using namespace ftxui;
        // to be done :)
        return vbox();
    }

public:
    void assignNewMap(observer_ptr<std::vector<std::vector<Tile>>> map)
    {
        game_map_ = map;
    }

    void assignNewPlayer(observer_ptr<Entity> player)
    {
        this->player_ = player;
    }
    ftxui::Element render()
    {
        using namespace ftxui;
        return hbox({

                   getSidebar(),
                   separatorLight(),
                   getMapBox(),
               }) |
               borderRounded | center | flex_shrink;
    }

    GameScreen(observer_ptr<std::vector<std::vector<Tile>>> map,
               observer_ptr<Entity>                         player,
               PositionSystem                              &pos_sys)
        : game_map_{map}, player_{player}, pos_system_{pos_sys}
    {
        using namespace ftxui;
        // colors
        auto wall_color         = Color::RGB(176, 176, 176);
        floor_color             = Color::RGB(66, 66, 66);
        auto floor_sprite_color = Color::RGB(88, 88, 88);
        auto stairs_color       = Color::RGB(22, 22, 22);
        auto weapon_color       = Color::RGB(240, 240, 240);
        auto armor_color        = Color::RGB(240, 240, 240);
        auto ring_color         = Color::RGB(255, 255, 0);
        auto potion_color       = Color::RGB(255, 50, 80);
        auto food_color         = Color::RGB(160, 80, 40);
        auto goal_item_color    = Color::RGB(255, 40, 255);
        auto player_color       = Color::RGB(255, 255, 255);
        auto rat_color          = Color::RGB(140, 140, 140);
        auto bat_color          = Color::RGB(140, 70, 35);
        auto spider_color       = Color::RGB(153, 0, 0);
        auto viper_color        = Color::RGB(50, 180, 80);
        auto kobold_color       = Color::RGB(204, 0, 0);
        auto goblin_color       = Color::RGB(80, 150, 10);
        auto orc_color          = Color::RGB(50, 150, 10);
        auto troll_color        = Color::RGB(120, 130, 150);
        auto zombie_color       = Color::RGB(140, 170, 140);
        auto deathknight_color  = Color::RGB(128, 128, 128);
        auto assassin_color =
            Color::RGB(100, 100, 100); // intentionally hard to see
        auto demon_color          = Color::RGB(140, 0, 70);
        auto vampire_color        = Color::RGB(230, 230, 230);
        auto dragon_color         = Color::RGB(51, 51, 255);
        auto venus_flytrap_color  = Color::RGB(0, 204, 0);

        // sprites
        auto wall_sprite          = ' ';
        auto floor_sprite         = '.';
        auto stairs_sprite        = '#';
        auto weapon_sprite        = '/';
        auto armor_sprite         = '%';
        auto ring_sprite          = '?';
        auto potion_sprite        = '!';
        auto food_sprite          = '*';
        auto goal_sprite          = '$';
        auto player_sprite        = '@';
        auto rat_sprite           = 'R';
        auto bat_sprite           = 'B';
        auto giant_spider_sprite  = 'S';
        auto viper_sprite         = 'V';
        auto kobold_sprite        = 'K';
        auto goblin_sprite        = 'G';
        auto orc_sprite           = 'O';
        auto troll_sprite         = 'T';
        auto zombie_sprite        = 'Z';
        auto deathknight_sprite   = 'D';
        auto assassin_sprite      = 'A';
        auto demon_sprite         = 'D';
        auto vampire_sprite       = 'V';
        auto dragon_sprite        = 'D';
        auto venus_flytrap_sprite = 'F';

        appearances_[TileAppearance::WALL] =
            Sprite(wall_sprite, wall_color, wall_color);
        appearances_[TileAppearance::FLOOR] =
            Sprite(floor_sprite, floor_sprite_color, floor_color);
        appearances_[TileAppearance::STAIRS] =
            Sprite(stairs_sprite, stairs_color, floor_color);
        appearances_[TileAppearance::WEAPON] =
            Sprite(weapon_sprite, weapon_color, floor_color);
        appearances_[TileAppearance::ARMOR] =
            Sprite(armor_sprite, armor_color, floor_color);
        appearances_[TileAppearance::RING] =
            Sprite(ring_sprite, ring_color, floor_color);
        appearances_[TileAppearance::POTION] =
            Sprite(potion_sprite, potion_color, floor_color);
        appearances_[TileAppearance::FOOD] =
            Sprite(food_sprite, food_color, floor_color);
        appearances_[TileAppearance::GOAL_ITEM] =
            Sprite(goal_sprite, goal_item_color, floor_color);
        appearances_[TileAppearance::PLAYER] =
            Sprite(player_sprite, player_color, floor_color);
        appearances_[TileAppearance::RAT] =
            Sprite(rat_sprite, rat_color, floor_color);
        appearances_[TileAppearance::BAT] =
            Sprite(bat_sprite, bat_color, floor_color);
        appearances_[TileAppearance::GIANT_SPIDER] =
            Sprite(giant_spider_sprite, spider_color, floor_color);
        appearances_[TileAppearance::VIPER] =
            Sprite(viper_sprite, viper_color, floor_color);
        appearances_[TileAppearance::KOBOLD] =
            Sprite(kobold_sprite, kobold_color, floor_color);
        appearances_[TileAppearance::GOBLIN] =
            Sprite(goblin_sprite, goblin_color, floor_color);
        appearances_[TileAppearance::ORC] =
            Sprite(orc_sprite, orc_color, floor_color);
        appearances_[TileAppearance::TROLL] =
            Sprite(troll_sprite, troll_color, floor_color);
        appearances_[TileAppearance::ZOMBIE] =
            Sprite(zombie_sprite, zombie_color, floor_color);
        appearances_[TileAppearance::DEATHKNIGHT] =
            Sprite(deathknight_sprite, deathknight_color, floor_color);
        appearances_[TileAppearance::ASSASSIN] =
            Sprite(assassin_sprite, assassin_color, floor_color);
        appearances_[TileAppearance::DEMON] =
            Sprite(demon_sprite, demon_color, floor_color);
        appearances_[TileAppearance::VAMPIRE] =
            Sprite(vampire_sprite, vampire_color, floor_color);
        appearances_[TileAppearance::DRAGON] =
            Sprite(dragon_sprite, dragon_color, floor_color);
        appearances_[TileAppearance::GIANT_VENUS_FLYTRAP] =
            Sprite(venus_flytrap_sprite, venus_flytrap_color, floor_color);
    }
};

#endif /*GAMESCREEN_H*/
