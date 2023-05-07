#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H
#include "../../components/components_all.h"
#include "../../effect.h"
#include "../../entity.h"
#include "../../rarity.h"
#include "../../system.h"
#include "../../tile.h"
#include <any>
#include <cstdint>
#include <list>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
class ItemFactory
{
    using GameMap = std::vector<std::vector<Tile>>;
    GameMap                                        &map_;
    int                                            &current_depth_;
    std::random_device                              rd_;
    std::mt19937                                    mt_engine_;
    std::uniform_int_distribution<>                 rarity_distro_;
    std::uniform_int_distribution<>                 item_amount_distro_;
    std::uniform_int_distribution<>                 weapon_amount_distro_;
    std::uniform_int_distribution<>                 armor_amount_distro_;
    std::uniform_int_distribution<>                 potion_amount_distro_;
    std::uniform_int_distribution<>                 food_amount_distro_;
    std::uniform_int_distribution<>                 x_pos_distro_;
    std::uniform_int_distribution<>                 y_pos_distro_;
    std::uniform_int_distribution<>                 weapon_crit_effect_distro_;
    std::uniform_int_distribution<>                 potion_effect_distro_;
    std::map<Rarity, int>                           rarity_chances_;
    std::map<Rarity, std::pair<uint16_t, uint16_t>> rarity_damage_ranges_;
    std::map<Rarity, std::pair<uint8_t, uint8_t>>   rarity_crit_chance_ranges_;
    std::map<Rarity, std::pair<double, double>> rarity_crit_multiplier_ranges_;
    std::unordered_map<int, Effect>             weapon_effect_map_;
    std::unordered_map<int, Effect>             potion_effect_map_;
    std::map<Rarity, std::pair<uint16_t, uint16_t>> rarity_armor_ranges_;
    std::unordered_map<Rarity, std::string>         rarity_weapon_names_;
    std::unordered_map<Rarity, std::string>         rarity_weapon_descritpions_;
    std::unordered_map<Rarity, std::string>         rarity_armor_names_;
    std::unordered_map<Rarity, std::string>         rarity_armor_descriptions_;
    std::unordered_map<Effect, std::string>         effect_potion_names_;
    std::unordered_map<Effect, std::string>         effect_potion_descriptions_;

    std::list<Entity *> generateWeapons(int amount)
    {
        std::list<Entity *> weapons;
        Rarity              weapon_rarity{};
        uint16_t            damage{};
        uint8_t             crit_chance{};
        double              crit_multiplier{};
        uint16_t            dmg_min{};
        uint16_t            dmg_max{};
        uint8_t             crit_chance_min{};
        uint8_t             crit_chance_max{};
        double              crit_multiplier_min{};
        double              crit_multiplier_max{};
        int                 roll{};
        int                 cumulated_rarity_chance{};
        std::string         name{};
        std::string         description{};

        std::uniform_int_distribution<>         damage_distro;
        std::uniform_int_distribution<uint16_t> crit_chance_distro;
        std::uniform_real_distribution<double>  crit_multiplier_distro;

        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            for (auto &[rarity, chance] : rarity_chances_)
            {
                if (roll <= chance + cumulated_rarity_chance)
                {
                    weapon_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }
            dmg_min         = rarity_damage_ranges_[weapon_rarity].first;
            dmg_max         = rarity_damage_ranges_[weapon_rarity].second;
            crit_chance_min = rarity_crit_chance_ranges_[weapon_rarity].first;
            crit_chance_max = rarity_crit_chance_ranges_[weapon_rarity].second;
            crit_multiplier_min =
                rarity_crit_multiplier_ranges_[weapon_rarity].first;
            crit_multiplier_max =
                rarity_crit_multiplier_ranges_[weapon_rarity].second;
            damage_distro = std::uniform_int_distribution<>(dmg_min, dmg_max);
            crit_chance_distro = std::uniform_int_distribution<uint16_t>(
                crit_chance_min, crit_chance_max);
            crit_multiplier_distro = std::uniform_real_distribution<double>(
                crit_multiplier_min, crit_multiplier_max);
            damage          = damage_distro(mt_engine_);
            crit_chance     = crit_chance_distro(mt_engine_);
            crit_multiplier = crit_multiplier_distro(mt_engine_);
            name            = rarity_weapon_names_[weapon_rarity];
            description     = rarity_weapon_descritpions_[weapon_rarity];
            Entity *weapon =
                new Entity(EntityType::ITEM,
                           {new Name(name),
                            new Description(description),
                            new WeaponComponent(damage),
                            new CritComponent(crit_chance, crit_multiplier)});
            weapons.emplace_back(weapon);
        }
    }
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

        rarity_chances_[Rarity::COMMON]                   = 45;
        rarity_chances_[Rarity::UNCOMMON]                 = 25;
        rarity_chances_[Rarity::RARE]                     = 15;
        rarity_chances_[Rarity::EPIC]                     = 10;
        rarity_chances_[Rarity::LEGENDARY]                = 5;

        // first number is minimum damage of a weapon with given rarity, second
        // is maximum
        rarity_damage_ranges_[Rarity::COMMON]             = {3, 6};
        rarity_damage_ranges_[Rarity::UNCOMMON]           = {7, 10};
        rarity_damage_ranges_[Rarity::RARE]               = {11, 18};
        rarity_damage_ranges_[Rarity::EPIC]               = {19, 25};
        rarity_damage_ranges_[Rarity::LEGENDARY]          = {26, 32};

        rarity_crit_chance_ranges_[Rarity::COMMON]        = {5, 6};
        rarity_crit_chance_ranges_[Rarity::UNCOMMON]      = {7, 8};
        rarity_crit_chance_ranges_[Rarity::RARE]          = {9, 14};
        rarity_crit_chance_ranges_[Rarity::EPIC]          = {15, 20};
        rarity_crit_chance_ranges_[Rarity::LEGENDARY]     = {21, 30};

        rarity_crit_multiplier_ranges_[Rarity::COMMON]    = {1.5l, 1.6l};
        rarity_crit_multiplier_ranges_[Rarity::UNCOMMON]  = {1.7l, 1.8l};
        rarity_crit_multiplier_ranges_[Rarity::RARE]      = {1.9l, 2.0l};
        rarity_crit_multiplier_ranges_[Rarity::EPIC]      = {2.1l, 2.3l};
        rarity_crit_multiplier_ranges_[Rarity::LEGENDARY] = {2.4l, 3.0l};

        weapon_effect_map_[0]                             = Effect::BLEED;
        weapon_effect_map_[1]                             = Effect::POISON;
        weapon_effect_map_[2]                             = Effect::BLIND;
        weapon_crit_effect_distro_ = std::uniform_int_distribution<>(0, 2);

        potion_effect_map_[0]      = Effect::HEAL | Effect::APPLY_ONCE;
        potion_effect_map_[1]      = Effect::HEAL; // regen
        potion_effect_map_[2]      = Effect::IRONSKIN | Effect::APPLY_ONCE;
        potion_effect_map_[3]      = Effect::STRENGTH | Effect::APPLY_ONCE;
        potion_effect_distro_      = std::uniform_int_distribution<>(0, 3);

        // analogously to damage ranges
        rarity_armor_ranges_[Rarity::COMMON]    = {11, 15};
        rarity_armor_ranges_[Rarity::UNCOMMON]  = {16, 20};
        rarity_armor_ranges_[Rarity::RARE]      = {21, 25};
        rarity_armor_ranges_[Rarity::EPIC]      = {26, 45};
        rarity_armor_ranges_[Rarity::LEGENDARY] = {46, 75};

        rarity_weapon_names_[Rarity::COMMON]    = "Sword";
        rarity_weapon_names_[Rarity::UNCOMMON]  = "Uncommon Sword";
        rarity_weapon_names_[Rarity::RARE]      = "Rare Sword";
        rarity_weapon_names_[Rarity::EPIC]      = "Epic Sword";
        rarity_weapon_names_[Rarity::LEGENDARY] = "Legendary Sword";

        rarity_weapon_descritpions_[Rarity::COMMON] =
            "A simple weapon, made by a mediocre blacksmith. It serves its "
            "purpose well enough";
        rarity_weapon_descritpions_[Rarity::UNCOMMON] =
            "A weapon made by a skilled craftsman. It has seen better days, "
            "but due to its quality, it's still a decent piece of weaponry";
        rarity_weapon_descritpions_[Rarity::RARE] =
            "This weapon was clearly made by a master of his craft. When you "
            "grip it, it feels as if it were a natural part of your arm";
        rarity_weapon_descritpions_[Rarity::EPIC] =
            "This weapon is of no ordinary origin. Its craftsmanship is truly "
            "exquisite, it seems to be in pristine condition despite the "
            "environment it has been in. A sword worthy of a king";
        rarity_weapon_descritpions_[Rarity::LEGENDARY] =
            "This sword is a weapon worthy of legendary heroes. It feels as if "
            "you could cut yourself by just looking at its edge. A weapon of "
            "otherwordly balance, when swung, the sword almost seems to "
            "possess a mind of its own";

        rarity_armor_names_[Rarity::COMMON]    = "Leather Armor";
        rarity_armor_names_[Rarity::UNCOMMON]  = "Chainmail Armor";
        rarity_armor_names_[Rarity::RARE]      = "Brigandine";
        rarity_armor_names_[Rarity::EPIC]      = "Splint Armor";
        rarity_armor_names_[Rarity::LEGENDARY] = "Plate Armor";

        rarity_armor_descriptions_[Rarity::COMMON] =
            "A crudely-made set of armor made of leather. Incapable of "
            "deflecting any but the weakest of blows";
        rarity_armor_descriptions_[Rarity::UNCOMMON] =
            "A decently set armor made of small, metal rings intertwined with "
            "each "
            "other. It can protect from cuts, but its comfort leaves a lot "
            "to be desired";
        rarity_armor_descriptions_[Rarity::RARE] =
            "A well-crafted set of armor made of small, metal plates riveted "
            "to a garment made of heavy cloth. Protects from attacks quite "
            "well while maintaining great mobility";
        rarity_armor_descriptions_[Rarity::EPIC] =
            "A masterfully-made set of armor made with long, narrow strips "
            "riveted to a leather garment. It offers great protection despite "
            "being lightweight without hindering mobility";
        rarity_armor_descriptions_[Rarity::LEGENDARY] =
            "An exquisitely-crafted, beautiful set of armor made of "
            "intertwining plates of metal. The armor offers unmatched "
            "protection and coverage while allowing its user for incredible "
            "mobility";
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
        return generatedItems;
    }
};
#endif /*ITEMFACTORY_H*/
