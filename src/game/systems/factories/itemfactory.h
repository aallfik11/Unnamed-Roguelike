#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H
#include "../../components/components_all.h"
#include "../../effect.h"
#include "../../entity.h"
#include "../../observerptr.h"
#include "../../rarity.h"
#include "../../system.h"
#include "../../tile.h"
#include <cstdint>
#include <list>
#include <map>
#include <random>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class MonsterFactory;

class ItemBuilder
{
    std::unordered_set<Component *> components;

public:
    [[nodiscard]] Entity *build();

    ItemBuilder &itemComponent(const ItemType type      = ItemType::NONE,
                               const uint16_t stack     = 1,
                               const uint16_t max_stack = 1,
                               const Rarity   rarity    = Rarity::COMMON,
                               const bool     equipped  = false);

    ItemBuilder &name(const std::string &name = "");
    ItemBuilder &description(const std::string &description = "");
    ItemBuilder &sprite(const TileAppearance appearance);
    ItemBuilder &damage(const uint16_t damage = 1);
    ItemBuilder &crit(const uint8_t                            crit_chance,
                      const double                             crit_multiplier,
                      std::initializer_list<EffectComponent *> effects);
    ItemBuilder &armor(const uint8_t armor_class = 10);
    ItemBuilder &buffs(std::initializer_list<EffectComponent *> buffs);
    ItemBuilder &hungerValue(const uint8_t saturation = 100);
};
class ItemFactory
{
    using GameMap = std::vector<std::vector<Tile>>;
    observer_ptr<GameMap>                           map_;
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

    friend class MonsterFactory;

    std::list<Entity *> generateWeapons(int amount);
    std::list<Entity *> generateArmors(int amount);
    std::list<Entity *> generateRings(int amount);
    std::list<Entity *> generatePotions(int amount);
    std::list<Entity *> generateFood(int amount);

public:
    ItemFactory(int &depth);

    void placeItem(observer_ptr<Entity> item);

    std::list<Entity *> generateItems() noexcept(false);

    Entity *generateWeapon(int rarity_modifier = 0);
    Entity *generateArmor(int rarity_modifier = 0);
    Entity *generateRing(int rarity_modifier = 0);
    Entity *generatePotion(int rarity_modifier = 0);
    Entity *generateFoodRation(int rarity_modifier = 0);

    std::list<Entity *> debugGenerateItems(int amount) noexcept(false);

    void assignMap(observer_ptr<GameMap> map);
};

#endif /*ITEMFACTORY_H*/
