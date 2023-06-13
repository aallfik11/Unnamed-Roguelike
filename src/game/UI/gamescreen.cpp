
#include "gamescreen.h"
#include "../components/amuletslot.h"
#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/buffcomponent.h"
#include "../components/experiencecomponent.h"
#include "../components/health.h"
#include "../components/hungercomponent.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/navmapcomponent.h"
#include "../components/tilecomponent.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../entity.h"
#include "../flagcomparator.h"
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

GameScreen::Sprite::Sprite(char         character,
                           ftxui::Color character_color,
                           ftxui::Color background_color)
{
    this->character        = character;
    this->character_color  = character_color;
    this->background_color = background_color;
}
ftxui::Element GameScreen::Sprite::returnRenderable()
{
    using namespace ftxui;
    return text(character) | color(character_color) | bgcolor(background_color);
}

inline ftxui::Color GameScreen::getColorByRarity(Rarity rarity) const
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

inline ftxui::Color GameScreen::getColorByEffect(Effect effect,
                                                 bool   is_text_color) const
{
    using namespace ftxui;
    effect &= ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
    switch (effect)
    {
    case Effect::HEAL:
        red   = 255;
        green = 204;
        blue  = 255;
        break;
    case Effect::BLEED:
        red   = 102;
        green = 0;
        blue  = 0;
        break;
    case Effect::BLIND:
        red   = 60;
        green = 60;
        blue  = 60;
        break;
    case Effect::POISON:
        red   = 64;
        green = 253;
        blue  = 20;
        break;
    case Effect::IRONSKIN:
        red   = 160;
        green = 160;
        blue  = 160;
        break;
    case Effect::STRENGTH:
        red   = 102;
        green = 0;
        blue  = 51;
        break;
    default:
        red   = 255;
        green = 255;
        blue  = 255;
        break;
    }
    if (is_text_color == true)
    {
        red   = 255 - red;
        green = 255 - green;
        blue  = 255 - blue;
    }
    return Color::RGB(red, green, blue);
}

inline std::string GameScreen::getEffectName(Effect effect) const
{
    auto effect_cpy = effect;
    effect_cpy &= ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
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

inline void GameScreen::calculateColumn(int              starting_index,
                                        int              index_guard,
                                        ftxui::Elements &cols)
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
            auto cell_tile_type       = (*game_map_)[x][y].type;
            auto cell_tile_appearance = (*game_map_)[x][y].appearance;

            if (u_compareFlags(cell_tile_type, {TileType::VISIBLE}) == false)
            {
                if (u_compareFlags(cell_tile_type, {TileType::MAPPED}) == false)
                {
                    cells[y] = appearances_[TileAppearance::UNMAPPED]
                                   .returnRenderable();
                    continue;
                }
                if (u_compareFlags(cell_tile_type, {TileType::HAS_STAIRS}))
                    [[unlikely]]
                {
                    cell_tile_appearance = TileAppearance::MAPPED_STAIRS;
                }
                else if (u_compareFlags(cell_tile_type, {TileType::WALL}))
                {
                    cell_tile_appearance = TileAppearance::MAPPED_WALL;
                }
                else if (u_compareFlags(cell_tile_type, {TileType::FLOOR}))
                {
                    cell_tile_appearance = TileAppearance::MAPPED_FLOOR;
                }
                cells[y] =
                    appearances_[cell_tile_appearance].returnRenderable();
                continue;
            }

            if (u_compareFlags(cell_tile_type,
                               {TileType::HAS_CREATURE, TileType::HAS_ITEM}) ==
                false)
            {

                cells[y] =
                    appearances_[cell_tile_appearance].returnRenderable();
                continue;
            }

            auto entities_at_coordinates =
                pos_system_.getEntitiesAtCoordinates(x, y);
            if (u_compareFlags(cell_tile_type, {TileType::HAS_CREATURE}))
            {
                if (auto creature =
                        std::ranges::find_if(entities_at_coordinates.begin(),
                                             entities_at_coordinates.end(),
                                             find_creature);
                    creature != entities_at_coordinates.end())
                {
                    auto creature_ptr = *creature;
                    cells[y] =
                        appearances_[creature_ptr->getComponent<TileComponent>()
                                         ->sprite]
                            .returnRenderable();
                }
            }
            else if (u_compareFlags(cell_tile_type, {TileType::HAS_ITEM}))
            {
                if (auto item =
                        std::ranges::find_if(entities_at_coordinates.begin(),
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
        }
        cols[x] = vbox(cells);
    }
}

ftxui::Element GameScreen::getMapBox()
{
    // std::mutex logger_mutex;
    // std::ofstream
    // logger(std::filesystem::current_path()/"thread_logger.txt",
    // std::ios::app);
    using namespace ftxui;
    Elements     columns(G_MAP_WIDTH);
    std::barrier sync(11);
    auto         work = [&](int starting_index, int index_guard)
    {
        // logger_mutex.lock();
        // logger << "Thread " << std::to_string(index_guard/10) << "
        // reporting for duty!" << std::endl; logger_mutex.unlock();
        calculateColumn(starting_index, index_guard, columns);
        // logger_mutex.lock();
        // logger << "Thread " << std::to_string(index_guard/10) << " has
        // finished work and arrived at the barrier" << std::endl;
        // logger_mutex.unlock();
        // sync.arrive_and_wait();
        // logger_mutex.lock();
        // logger << "Thread " << std::to_string(index_guard/10) << " has
        // passed the barrier" << std::endl; logger_mutex.unlock();
    };

    std::list<std::thread> workers;
    for (int i = 0; i < 10; ++i)
    {
        int index = i * 10;
        workers.emplace_back(std::thread(work, index, index + 10));
    }
    // sync.arrive_and_wait();
    for (auto &worker : workers)
    {
        worker.join();
    }
    // logger_mutex.lock();
    // logger << "==>All workers have finished their job successfully" <<
    // std::endl; logger_mutex.unlock();

    return hbox(columns);
}
ftxui::Element GameScreen::getSidebar()
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

inline ftxui::Element GameScreen::getPlayerHpBar()
{
    using namespace ftxui;
    auto player_hp  = player_->getComponent<Health>();
    auto hp_percent = player_hp->current_health_points /
                      static_cast<float>(player_hp->max_health_points);
    auto player_coords = player_->getComponent<Coordinates>();

    return vbox({text("x: " + std::to_string(player_coords->x) +
                      " y: " + std::to_string(player_coords->y)),
                 text(std::to_string(player_hp->current_health_points) + " / " +
                      std::to_string(player_hp->max_health_points)) |
                     hcenter,
                 gaugeRight(hp_percent) | color(Color::RGB(255, 0, 0)) |
                     borderRounded});
}

inline ftxui::Element GameScreen::getPlayerXpBar()
{
    using namespace ftxui;
    auto player_xp     = player_->getComponent<ExperienceComponent>();
    // auto adjusted_xp = player_xp->current_experience;
    // for (int i = 2; i <= player_xp->level;
    //      i++) // i starts at 2 since player starts at lvl 1 and 25
    //           // experience for level 1 should not be deduced
    // {
    //     adjusted_xp -= i * 25;
    // }
    auto xp_percentage = player_xp->current_experience /
                         static_cast<float>((player_xp->level + 1) * 25);
    return vbox(
        {text(std::to_string(player_xp->current_experience) + " / " +
              std::to_string((player_xp->level + 1) * 25)),
         hbox({text(std::to_string(player_xp->level)) | borderRounded,
               gaugeRight(xp_percentage) | color(Color::YellowLight) |
                   borderRounded,
               text(std::to_string(player_xp->level + 1)) | borderRounded})});
}

inline ftxui::Element GameScreen::getPlayerHungerBar()
{
    using namespace ftxui;
    auto player_hunger = player_->getComponent<HungerComponent>();
    auto hunger_percent =
        player_hunger->hunger / static_cast<float>(player_hunger->max_hunger);

    return vbox({text("Nutrition: " + std::to_string(player_hunger->hunger) +
                      " / " + std::to_string(player_hunger->max_hunger)),
                 gaugeRight(hunger_percent) | color(Color::RGB(155, 103, 60)) |
                     borderRounded});
}

inline ftxui::Element GameScreen::getPlayerAttackRating()
{
    using namespace ftxui;
    std::string attack_damage{};
    if (auto player_weapon = player_->getComponent<WeaponSlot>()->weapon_item)
    {
        attack_damage = std::to_string(
            player_weapon->getComponent<WeaponComponent>()->damage);
    }
    else
        attack_damage =
            std::to_string(player_->getComponent<WeaponComponent>()->damage);
    return vbox(text("Attack Damage"), text(attack_damage)) | borderRounded;
}

inline ftxui::Element GameScreen::getPlayerArmorRating()
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

inline ftxui::Element GameScreen::getPlayerEquipment()
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
    if (equipment.empty())
    {
        return vbox();
    }
    return vbox(equipment) | borderRounded;
}

inline ftxui::Element GameScreen::getPlayerEffects()
{
    using namespace ftxui;
    Elements buffs;
    for (auto &buff : player_->getConstComponent<BuffComponent>()->buffs)
    {
        if (buff.second->effect_duration == 0)
        {
            continue;
        }
        buffs.push_back(
            text(getEffectName(buff.first) + " " +
                 std::to_string((buff.second->effect_strength)) + " - " +
                 std::to_string(buff.second->effect_duration) + "t") |
            hcenter | color(getColorByEffect(buff.first, true)) |
            bgcolor(getColorByEffect(buff.first)));
    }
    if (buffs.empty())
    {
        return vbox();
    }
    return vbox(buffs) | borderRounded;
}

inline ftxui::Element GameScreen::getLastDamagedEnemy()
{
    using namespace ftxui;
    // to be done :)
    return vbox();
}

ftxui::Element GameScreen::debugGetMapBox(observer_ptr<Entity> entity)
{
    using namespace ftxui;

    auto        nav_map   = entity->getComponent<NavMapComponent>()->nav_map;
    auto        coord_ptr = entity->getComponent<Coordinates>();
    auto        x_coord   = coord_ptr->x;
    auto        y_coord   = coord_ptr->y;
    auto        player_coords = player_->getComponent<Coordinates>();
    std::string sprite        = " ";
    Elements    cols;
    for (int x = 0; x < 100; x++)
    {
        Elements rows;
        for (int y = 0; y < 50; y++)
        {

            auto    score = nav_map[x][y].score;
            uint8_t red   = 0;
            uint8_t green = 0;
            uint8_t blue  = 0;
            if (score != ~0)
            {
                auto score_backup  = score;
                score             /= 3;
                if (score <= 255)
                {
                    red   = 255;
                    green = score;
                }
                else if (score > 255 && score <= 511)
                {
                    red   = 255 - score % 256;
                    green = 255;
                    blue  = score % 256;
                }
                else if (score > 511 && score <= 767)
                {
                    green = 255 - score % 256;
                    blue  = 255;
                }
                else
                    blue = 255;

                score = score_backup;
            }

            if (x == player_coords->x && y == player_coords->y)
            {
                rows.push_back(text("@") |
                               /* color(entity->getComponent<TileComponent>()
                                         ->tile.color) | */
                               color(Color::Grey15) | bgcolor(Color::Black));
            }
            else if (x == x_coord && y == y_coord)
            {
                rows.push_back(text("M") |
                               /* color(entity->getComponent<TileComponent>()
                                         ->tile.color) | */
                               color(Color::Grey15) | bgcolor(Color::Black));
            }
            else if (score == 0)
            {
                if (((*game_map_)[x][y].type & TileType::WALL) ==
                    TileType::NONE)
                    rows.push_back(text("$") |
                                   bgcolor(Color::RGB(red, green, blue)));
                else
                    rows.push_back(text("$") | bgcolor(Color::BlueViolet));
            }
            else if (score == ~0)
            {
                rows.push_back(text(" ") | bgcolor(Color::GrayDark));
            }
            else if (((*game_map_)[x][y].type & TileType::HAS_CREATURE) !=
                     TileType::NONE)
            {

                rows.push_back(text("M") |
                               /* color(entity->getComponent<TileComponent>()
                                         ->tile.color) | */
                               color(Color::Purple) | bgcolor(Color::Black));
            }
            else
                rows.push_back(text(std::to_string(score % 9 + 1)) |
                               bgcolor(Color::RGB(red, green, blue)));
        }

        cols.push_back(vbox(rows));
    }
    return vbox(hbox(cols));
    // return vbox(text(rg_l1),
    //             text(rg_l2),
    //             text(rg_l3),
    //             text(rg_l4),
    //             text(rg_l5),
    //             text(rg_l6),
    //             text(rg_l7)) |
    //        hcenter;
}

void GameScreen::assignNewMap(observer_ptr<std::vector<std::vector<Tile>>> map)
{
    game_map_ = map;
}

void GameScreen::assignNewPlayer(observer_ptr<Entity> player)
{
    this->player_ = player;
}
ftxui::Element GameScreen::render()
{
    using namespace ftxui;
    auto flexbox_config = FlexboxConfig();
    flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);

    auto controls = flexbox({text("ESC - Exit To Menu"),
                             text("I - Open Inventory"),
                             text("S - Save Game"),
                             text("L - Open Logs"),
                             text("R - Rest")},
                            flexbox_config);

    return vbox({hbox({

                     getSidebar(),
                     separatorLight(),
                     getMapBox(),
                 }) | flex_shrink |
                     center,
                 separatorLight(),
                 controls | flex_shrink | hcenter}) |
           flex_shrink | borderRounded | center;
}

ftxui::Element GameScreen::debugRender(observer_ptr<Entity> enemy)
{

    using namespace ftxui;
    auto coords        = enemy->getComponent<Coordinates>();
    auto x_coord       = coords->x;
    auto y_coord       = coords->y;
    auto nav_map       = enemy->getComponent<NavMapComponent>()->nav_map;
    auto monster_score = nav_map[x_coord][y_coord].score;
    auto score_up      = nav_map[x_coord][y_coord - 1].score;
    auto score_down    = nav_map[x_coord][y_coord + 1].score;
    auto score_left    = nav_map[x_coord - 1][y_coord].score;
    auto score_right   = nav_map[x_coord + 1][y_coord].score;
    auto scores        = vbox({
        text("@:" + std::to_string(monster_score)),
        text("^:" + std::to_string(score_up)),
        text("\\/:" + std::to_string(score_down)),
        text("<-:" + std::to_string(score_left)),
        text("->:" + std::to_string(score_right)),
    });

    return hbox({

               getSidebar(),
               separatorLight(),
               debugGetMapBox(enemy),
               separatorLight(),
               scores,
           }) |
           borderRounded | center | flex_shrink;
}

GameScreen::GameScreen(observer_ptr<std::vector<std::vector<Tile>>> map,
                       observer_ptr<Entity>                         player,
                       PositionSystem                              &pos_sys)
    : game_map_{map}, player_{player}, pos_system_{pos_sys}
{
    using namespace ftxui;
    // colors
    auto wall_color                = Color::RGB(176, 176, 176);
    auto floor_color               = Color::RGB(66, 66, 66);
    auto floor_sprite_color        = Color::RGB(88, 88, 88);
    auto stairs_color              = Color::RGB(200, 200, 200);
    auto mapped_wall_color         = Color::RGB(160, 160, 160);
    auto mapped_floor_color        = Color::RGB(50, 50, 50);
    auto mapped_floor_sprite_color = Color::RGB(66, 66, 66);
    auto mapped_stairs_color       = Color::RGB(184, 184, 184);
    auto weapon_color              = Color::RGB(240, 240, 240);
    auto armor_color               = Color::RGB(240, 240, 240);
    auto ring_color                = Color::RGB(255, 255, 0);
    auto potion_color              = Color::RGB(255, 50, 80);
    auto food_color                = Color::RGB(160, 80, 40);
    auto goal_item_color           = Color::RGB(255, 40, 255);
    auto player_color              = Color::RGB(255, 255, 255);
    auto rat_color                 = Color::RGB(140, 140, 140);
    auto bat_color                 = Color::RGB(140, 70, 35);
    auto spider_color              = Color::RGB(153, 0, 0);
    auto viper_color               = Color::RGB(50, 180, 80);
    auto kobold_color              = Color::RGB(204, 0, 0);
    auto goblin_color              = Color::RGB(80, 150, 10);
    auto orc_color                 = Color::RGB(50, 150, 10);
    auto troll_color               = Color::RGB(120, 130, 150);
    auto zombie_color              = Color::RGB(140, 170, 140);
    auto deathknight_color         = Color::RGB(128, 128, 128);
    auto assassin_color =
        Color::RGB(100, 100, 100); // intentionally hard to see
    auto demon_color          = Color::RGB(140, 0, 70);
    auto vampire_color        = Color::RGB(230, 230, 230);
    auto dragon_color         = Color::RGB(51, 51, 255);
    auto venus_flytrap_color  = Color::RGB(0, 204, 0);

    // sprites
    auto wall_sprite          = ' ';
    auto floor_sprite         = '.';
    auto mapped_floor_sprite  = ' ';
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
    appearances_[TileAppearance::MAPPED_WALL] =
        Sprite(wall_sprite, mapped_wall_color, mapped_wall_color);
    appearances_[TileAppearance::MAPPED_FLOOR] = Sprite(
        mapped_floor_sprite, mapped_floor_sprite_color, mapped_floor_color);
    appearances_[TileAppearance::MAPPED_STAIRS] =
        Sprite(stairs_sprite, mapped_stairs_color, mapped_floor_color);
    appearances_[TileAppearance::UNMAPPED] =
        Sprite(wall_sprite, Color::Black, Color::Black);
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
