#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H
#include "../../components/components_all.h"
#include "../../effect.h"
#include "../../entity.h"
#include "../../rarity.h"
#include "../../system.h"
#include "../../tile.h"
#include <any>
#include <list>
#include <map>
#include <random>
#include <vector>
class ItemFactory
{
    using GameMap = std::vector<std::vector<Tile>>;
    GameMap                              &map_;
    int                                  &current_depth_;
    std::random_device                    rd_;
    std::mt19937                          mt_engine_;
    std::uniform_int_distribution<>       rarity_distro_;
    std::uniform_int_distribution<>       item_amount_distro_;
    std::uniform_int_distribution<>       weapon_amount_distro_;
    std::uniform_int_distribution<>       armor_amount_distro_;
    std::uniform_int_distribution<>       potion_amount_distro_;
    std::uniform_int_distribution<>       food_amount_distro_;
    std::uniform_int_distribution<>       x_pos_distro_;
    std::uniform_int_distribution<>       y_pos_distro_;
    std::map<Rarity, int>                 rarity_chances_;
    std::map<Rarity, std::pair<int, int>> rarity_damage_ranges_;
    std::map<Rarity, std::pair<int, int>> rarity_armor_ranges_;

    std::list<Entity *> generateWeapons(int amount) {}
    std::list<Entity *> generateArmors(int amount) {}
    std::list<Entity *> generatePotions(int amount) {}
    std::list<Entity *> generateFood(int amount) {}

public:
    ItemFactory(GameMap &map, int &depth) : map_{map}, current_depth_{depth}
    {
        mt_engine_            = std::mt19937(rd_());
        rarity_distro_        = std::uniform_int_distribution<>(1, 100);
        item_amount_distro_   = std::uniform_int_distribution<>(6, 15);
        weapon_amount_distro_ = std::uniform_int_distribution<>(0, 5);
        armor_amount_distro_  = std::uniform_int_distribution<>(0, 5);
        potion_amount_distro_ = std::uniform_int_distribution<>(0, 4);
        food_amount_distro_   = std::uniform_int_distribution<>(2, 6);
        x_pos_distro_ = std::uniform_int_distribution<>(1, map_.size() - 1);
        y_pos_distro_ = std::uniform_int_distribution<>(1, map_[0].size() - 1);

        rarity_chances_[Rarity::COMMON]          = 45;
        rarity_chances_[Rarity::UNCOMMON]        = 25;
        rarity_chances_[Rarity::RARE]            = 15;
        rarity_chances_[Rarity::EPIC]            = 10;
        rarity_chances_[Rarity::LEGENDARY]       = 5;

        // first number is minimum damage of a weapon with given rarity, second
        // is maximum
        rarity_damage_ranges_[Rarity::COMMON]    = {3, 6};
        rarity_damage_ranges_[Rarity::UNCOMMON]  = {7, 10};
        rarity_damage_ranges_[Rarity::RARE]      = {11, 18};
        rarity_damage_ranges_[Rarity::EPIC]      = {19, 25};
        rarity_damage_ranges_[Rarity::LEGENDARY] = {26, 32};

        // analogously to damage ranges
        rarity_armor_ranges_[Rarity::COMMON]     = {11, 15};
        rarity_armor_ranges_[Rarity::COMMON]     = {16, 20};
        rarity_armor_ranges_[Rarity::COMMON]     = {21, 25};
        rarity_armor_ranges_[Rarity::COMMON]     = {26, 45};
        rarity_armor_ranges_[Rarity::COMMON]     = {46, 75};
    }

    std::list<Entity *> generateItems()
    {
        std::list<Entity *> generatedItems;
        auto                weapon_amount = weapon_amount_distro_(mt_engine_);
        auto                armor_amount  = armor_amount_distro_(mt_engine_);
        auto                potion_amount = potion_amount_distro_(mt_engine_);
        auto                food_amount   = food_amount_distro_(mt_engine_);

        auto weapons                      = generateWeapons(weapon_amount);
        auto armors                       = generateArmors(armor_amount);
        auto potions                      = generatePotions(potion_amount);
        auto food                         = generateFood(food_amount);
        generatedItems.splice(generatedItems.end(), weapons);
        generatedItems.splice(generatedItems.end(), armors);
        generatedItems.splice(generatedItems.end(), potions);
        generatedItems.splice(generatedItems.end(), food);
        for (auto &entity : generatedItems)
        {
            auto entity_message = {
                std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
                std::make_any<Entity *>(entity)};
            auto pos_message = {std::make_any<SystemAction::POSITION>(
                                    SystemAction::POSITION::ADD),
                                std::make_any<Entity *>(entity)};
            System::sendSystemMessage(SystemType::ENTITY, entity_message);
            System::sendSystemMessage(SystemType::POSITION, pos_message);
        }
    }
};
#endif /*ITEMFACTORY_H*/
