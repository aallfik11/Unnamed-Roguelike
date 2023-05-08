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
    std::uniform_int_distribution<>                 ring_amount_distro_;
    std::uniform_int_distribution<>                 potion_amount_distro_;
    std::uniform_int_distribution<>                 food_amount_distro_;
    std::uniform_int_distribution<>                 x_pos_distro_;
    std::uniform_int_distribution<>                 y_pos_distro_;
    std::uniform_int_distribution<>                 weapon_crit_effect_distro_;
    std::uniform_int_distribution<>                 potion_effect_distro_;
    std::uniform_int_distribution<>                 ring_effect_distro_;
    std::map<Rarity, int>                           rarity_chances_;
    std::map<Rarity, std::pair<uint16_t, uint16_t>> rarity_damage_ranges_;
    std::map<Rarity, std::pair<uint8_t, uint8_t>>   rarity_crit_chance_ranges_;
    std::map<Rarity, std::pair<double, double>> rarity_crit_multiplier_ranges_;
    std::unordered_map<int, Effect>             weapon_effect_map_;
    std::unordered_map<int, Effect>             potion_effect_map_;
    std::unordered_map<int, Effect>             ring_effect_map_;
    std::map<Rarity, std::pair<uint16_t, uint16_t>> rarity_armor_ranges_;
    std::unordered_map<Rarity, std::string>         rarity_weapon_names_;
    std::unordered_map<Rarity, std::string>         rarity_weapon_descritpions_;
    std::unordered_map<Rarity, std::string>         rarity_armor_names_;
    std::unordered_map<Rarity, std::string>         rarity_armor_descriptions_;
    std::unordered_map<Effect, std::string>         effect_ring_names_;
    std::unordered_map<Effect, std::string>         effect_ring_descriptions_;
    std::unordered_map<Effect, std::string>         effect_potion_names_;
    std::unordered_map<Effect, std::string>         effect_potion_descriptions_;
    std::unordered_map<Rarity, uint8_t>             rarity_food_values_;
    std::unordered_map<Rarity, std::string>         rarity_food_names_;
    std::unordered_map<Rarity, std::string>         rarity_food_descriptions_;

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

        std::uniform_int_distribution<uint16_t> damage_distro;
        std::uniform_int_distribution<uint16_t> crit_chance_distro;
        std::uniform_real_distribution<double>  crit_multiplier_distro;

        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            cumulated_rarity_chance = 0;
            for (auto &[rarity, chance] : rarity_chances_)
            {
                if (roll <= chance + cumulated_rarity_chance)
                {
                    weapon_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }
            BuffComponent *weapon_buffs = new BuffComponent;
            if (weapon_rarity >= Rarity::RARE)
            {
                roll = weapon_crit_effect_distro_(mt_engine_);
                if (roll <= 2)
                {
                    auto effect   = weapon_effect_map_[roll];
                    auto strength = static_cast<uint8_t>(weapon_rarity) * 2;
                    auto duration = static_cast<uint16_t>(weapon_rarity);
                    weapon_buffs->buffs[(
                        effect & ~(Effect::APPLIED | Effect::APPLY_ONCE))] =
                        std::make_unique<EffectComponent>(
                            effect, strength, duration);
                }
            }

            dmg_min         = rarity_damage_ranges_[weapon_rarity].first;
            dmg_max         = rarity_damage_ranges_[weapon_rarity].second;

            crit_chance_min = rarity_crit_chance_ranges_[weapon_rarity].first;
            crit_chance_max = rarity_crit_chance_ranges_[weapon_rarity].second;

            crit_multiplier_min =
                rarity_crit_multiplier_ranges_[weapon_rarity].first;
            crit_multiplier_max =
                rarity_crit_multiplier_ranges_[weapon_rarity].second;

            damage_distro =
                std::uniform_int_distribution<uint16_t>(dmg_min, dmg_max);

            crit_chance_distro = std::uniform_int_distribution<uint16_t>(
                crit_chance_min, crit_chance_max);
            crit_multiplier_distro = std::uniform_real_distribution<double>(
                crit_multiplier_min, crit_multiplier_max);

            damage          = damage_distro(mt_engine_);
            crit_chance     = crit_chance_distro(mt_engine_);
            crit_multiplier = crit_multiplier_distro(mt_engine_);
            name            = rarity_weapon_names_[weapon_rarity];
            description     = rarity_weapon_descritpions_[weapon_rarity];
            Entity *weapon  = new Entity(
                EntityType::ITEM,
                {new ItemComponent(ItemType::WEAPON, 1, 1, weapon_rarity),
                  new Name(name),
                  new Description(description),
                  new WeaponComponent(damage),
                  new CritComponent(
                     crit_chance, crit_multiplier, weapon_buffs)});
            weapons.emplace_back(weapon);
        }
        return weapons;
    }
    std::list<Entity *> generateArmors(int amount)
    {
        std::list<Entity *> armors;
        Rarity              armor_rarity{};
        uint8_t             armor_class{};
        uint8_t             ac_min{};
        uint8_t             ac_max{};
        int                 roll{};
        int                 cumulated_rarity_chance{};
        std::string         name{};
        std::string         description{};

        std::uniform_int_distribution<uint8_t> armor_distro;
        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            cumulated_rarity_chance = 0;

            for (auto &[rarity, chance] : rarity_chances_)
            {
                if (roll <= chance + cumulated_rarity_chance)
                {
                    armor_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }

            ac_min = rarity_armor_ranges_[armor_rarity].first;
            ac_max = rarity_armor_ranges_[armor_rarity].second;
            armor_distro =
                std::uniform_int_distribution<uint8_t>(ac_min, ac_max);
            armor_class = armor_distro(mt_engine_);

            name        = rarity_armor_names_[armor_rarity];
            description = rarity_armor_descriptions_[armor_rarity];
            armors.emplace_back(new Entity(
                EntityType::ITEM,
                {new ItemComponent(ItemType::ARMOR, 1, 1, armor_rarity),
                 new ArmorComponent(armor_class),
                 new Name(name),
                 new Description(description)}));
        }
        return armors;
    }
    std::list<Entity *> generateRings(int amount)
    {
        std::list<Entity *> rings;
        Rarity              ring_rarity{};
        std::string         name{};
        std::string         description{};
        int                 roll{};
        int                 cumulated_rarity_chance{};
        Effect              effect{};
        uint8_t             effect_strength{};

        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            cumulated_rarity_chance = 0;
            for (auto &[rarity, chance] : rarity_chances_)
            {

                if (roll <= chance + cumulated_rarity_chance)
                {
                    ring_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }
            roll             = ring_effect_distro_(mt_engine_);
            effect           = ring_effect_map_[roll];
            effect_strength  = static_cast<uint8_t>(ring_rarity) * 2;
            name             = effect_ring_names_[effect];
            description      = effect_ring_descriptions_[effect];
            effect          |= Effect::PERMANENT;
            rings.emplace_back(new Entity(
                EntityType::ITEM,
                {new ItemComponent(ItemType::RING, 1, 1, ring_rarity),
                 new Name(name),
                 new Description(description),
                 new BuffComponent(
                     {new EffectComponent(effect, effect_strength)})}));
        }
        return rings;
    }
    std::list<Entity *> generatePotions(int amount)
    {
        std::list<Entity *> potions;
        Rarity              potion_rarity{};
        std::string         name{};
        std::string         description{};
        int                 roll{};
        int                 cumulated_rarity_chance{};
        Effect              effect{};
        uint8_t             effect_strength{};
        uint16_t            effect_duration{};
        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            cumulated_rarity_chance = 0;
            for (auto &[rarity, chance] : rarity_chances_)
            {

                if (roll <= chance + cumulated_rarity_chance)
                {
                    potion_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }
            effect_strength = static_cast<uint8_t>(potion_rarity);
            effect_duration = static_cast<uint16_t>(potion_rarity) * 2;
            roll            = potion_effect_distro_(mt_engine_);
            effect          = potion_effect_map_[roll];
            name            = effect_potion_names_[effect];
            description     = effect_potion_descriptions_[effect];
            potions.emplace_back(new Entity(
                EntityType::ITEM,
                {new ItemComponent(ItemType::POTION, 1, 4, potion_rarity),
                 new Name(name),
                 new Description(description),
                 new BuffComponent({new EffectComponent(
                     effect, effect_strength, effect_duration)})}));
        }
        return potions;
    }
    std::list<Entity *> generateFood(int amount)
    {
        std::list<Entity *> food;
        Rarity              food_rarity{};
        int                 roll{};
        int                 cumulated_rarity_chance{};
        std::string         name{};
        std::string         description{};
        uint8_t             hunger_replenished{};
        for (int i = 0; i < amount; i++)
        {
            roll = rarity_distro_(mt_engine_);
            roll = (roll + current_depth_ >= 100) ? 100 : roll + current_depth_;
            cumulated_rarity_chance = 0;
            for (auto &[rarity, chance] : rarity_chances_)
            {
                if (roll <= chance + cumulated_rarity_chance)
                {
                    food_rarity = rarity;
                    break;
                }
                cumulated_rarity_chance += chance;
            }
            name               = rarity_food_names_[food_rarity];
            description        = rarity_food_descriptions_[food_rarity];
            hunger_replenished = rarity_food_values_[food_rarity];
            food.emplace_back(new Entity(
                EntityType::ITEM,
                {new ItemComponent(ItemType::FOOD, 1, 8, food_rarity),
                 new HungerComponent(hunger_replenished),
                 new Name(name),
                 new Description(description)}));
        }
        return food;
    }

public:
    ItemFactory(GameMap &map, int &depth) : map_{map}, current_depth_{depth}
    {
        mt_engine_            = std::mt19937(rd_());
        rarity_distro_        = std::uniform_int_distribution<>(1, 100);
        item_amount_distro_   = std::uniform_int_distribution<>(6, 15);
        weapon_amount_distro_ = std::uniform_int_distribution<>(0, 5);
        armor_amount_distro_  = std::uniform_int_distribution<>(0, 5);
        ring_amount_distro_   = std::uniform_int_distribution<>(0, 1);
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
        weapon_crit_effect_distro_ = std::uniform_int_distribution<>(0, 5);

        ring_effect_map_[0]        = Effect::IRONSKIN | Effect::APPLY_ONCE;
        ring_effect_map_[1]        = Effect::STRENGTH | Effect::APPLY_ONCE;
        ring_effect_distro_        = std::uniform_int_distribution<>(0, 1);

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

        effect_ring_names_[Effect::STRENGTH] = "Ring of Strength";
        effect_ring_names_[Effect::IRONSKIN] = "Effect of Iron Skin";

        effect_ring_descriptions_[Effect::STRENGTH] =
            "When you put this ring on your finger, you feel as if you could "
            "pulverize rocks to dust with your bare hands";
        effect_ring_descriptions_[Effect::IRONSKIN] =
            "When you put this ring on your finger, you feel your skin "
            "becoming impervious to blows";

        effect_potion_names_[(Effect::HEAL | Effect::APPLY_ONCE)] =
            "Potion of Healing";
        effect_potion_names_[Effect::HEAL] = "Potion of Regeneration";
        effect_potion_names_[(Effect::IRONSKIN | Effect::APPLY_ONCE)] =
            "Potion of Iron Skin";
        effect_potion_names_[(Effect::STRENGTH | Effect::APPLY_ONCE)] =
            "Potion of Strength";

        effect_potion_descriptions_[(Effect::HEAL | Effect::APPLY_ONCE)] =
            "A ruby-colored concoction, its smell reminiscent of fermented "
            "fruits. Heals your wounds when drunk";
        effect_potion_descriptions_[Effect::HEAL] =
            "A light-pink liquid. Its smell reminds you of rose petals. Slowly "
            "heals over a duration of time";
        effect_potion_descriptions_[(Effect::IRONSKIN | Effect::APPLY_ONCE)] =
            "A metallic liquid, looking almost like mercury. It smells "
            "metallic. It seems that drinking this potion makes your skin more "
            "resistant to attacks";
        effect_potion_descriptions_[(Effect::STRENGTH | Effect::APPLY_ONCE)] =
            "A dark, crimson concotion, its color reminiscent dried blood. It "
            "smells of alcohol. The potion increases the drinker's strenght";

        rarity_food_values_[Rarity::COMMON]    = 10;
        rarity_food_values_[Rarity::UNCOMMON]  = 15;
        rarity_food_values_[Rarity::RARE]      = 30;
        rarity_food_values_[Rarity::EPIC]      = 60;
        rarity_food_values_[Rarity::LEGENDARY] = 100;

        rarity_food_names_[Rarity::COMMON]     = "Tiny Food Ration";
        rarity_food_names_[Rarity::UNCOMMON]   = "Small Food Ration";
        rarity_food_names_[Rarity::RARE]       = "Medium Food Ration";
        rarity_food_names_[Rarity::EPIC]       = "Large Food Ration";
        rarity_food_names_[Rarity::LEGENDARY]  = "Lavish Food Ration";

        rarity_food_descriptions_[Rarity::COMMON] =
            "A ration partially eaten away by the rats. After cutting "
            "off the bad part , not much of it remains but it still seems to "
            "be edible";
        rarity_food_descriptions_[Rarity::UNCOMMON] =
            "A small food ration of dried meat and hardtack. Preserved quite "
            "well despite its location, it's enough to keep you going for a "
            "little while";
        rarity_food_descriptions_[Rarity::RARE] =
            "A medium-sized food ration consisting of a decent quantity of "
            "dried fruits, meat and hardtack. It seems to be quite edible "
            "despite its age";
        rarity_food_descriptions_[Rarity::EPIC] =
            "A large ration fit for a travelling knight, made of high quality "
            "ingredients and packed nicely. Looks as fresh as if it were "
            "prepared yesterday";
        rarity_food_descriptions_[Rarity::LEGENDARY] =
            "A feast packed inside a sealed wooden box. This meal is worthy of "
            "a king, and it's more than capable of filling you completely";
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

    std::list<Entity *> debugGenerateItems(int amount)
    {
        std::list<Entity *> generatedItems;
        auto                weapons = generateWeapons(amount);
        auto                armors  = generateArmors(amount);
        auto                potions = generatePotions(amount);
        auto                food    = generateFood(amount);
        generatedItems.splice(generatedItems.end(), weapons);
        generatedItems.splice(generatedItems.end(), armors);
        generatedItems.splice(generatedItems.end(), potions);
        generatedItems.splice(generatedItems.end(), food);
        return generatedItems;
    }
};
#endif /*ITEMFACTORY_H*/
