#ifndef MONSTERFACTORY_H
#define MONSTERFACTORY_H
#include "../../components/components_all.h"
#include "../../monsters.h"
#include "../../system.h"
#include "../../tile.h"
#include "itemfactory.h"
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <list>
#include <memory>
#include <random>
#include <stdint.h>
#include <unordered_map>
#include <vector>

class MonsterFactory
{
    using Monsters::ASSASSIN;
    using Monsters::BAT;
    using Monsters::DEATHKNIGHT;
    using Monsters::DEMON;
    using Monsters::DRAGON;
    using Monsters::GIANT_SPIDER;
    using Monsters::GIANT_VENUS_FLYTRAP;
    using Monsters::GOBLIN;
    using Monsters::KOBOLD;
    using Monsters::ORC;
    using Monsters::RAT;
    using Monsters::TROLL;
    using Monsters::VAMPIRE;
    using Monsters::VIPER;
    using Monsters::ZOMBIE;

    ItemFactory                                 &item_factory_;
    observer_ptr<std::vector<std::vector<Tile>>> map_;
    int                                         &current_depth_;
    std::vector<Monsters>                        low_level_monsters_;
    std::vector<Monsters>                        med_level_monsters;
    std::vector<Monsters>                        high_level_monsters_;
    std::random_device                           rd_;
    std::mt19937                                 mt_engine_;

    std::uniform_int_distribution<> monster_amount_distro_;
    std::uniform_int_distribution<> low_level_monster_amount_distro_;
    std::uniform_int_distribution<> med_level_monster_amount_distro_;
    std::uniform_int_distribution<> high_level_monster_amount_distro_;
    std::uniform_int_distribution<> random_low_lvl_monster_;
    std::uniform_int_distribution<> random_med_lvl_monster_;
    std::uniform_int_distribution<> random_high_lvl_monster_;

    std::unordered_map<Monsters,
                       std::function<void(MonsterFactory *, Entity *const)>>
        monster_generators_;

    class MonsterBuilder
    {
        bool    is_humanoid_;
        Entity *monster_;

    public:
        MonsterBuilder(Entity *const monster);

        MonsterBuilder &start(Entity *const monster);

        MonsterBuilder &humanoid();

        MonsterBuilder &name(const std::string &name);
        MonsterBuilder &ai(const AIType &ai_type);
        MonsterBuilder &los(const uint16_t seeing_distance);
        MonsterBuilder &health(const uint16_t max_hp);
        MonsterBuilder &baseDamage(const uint16_t damage);
        MonsterBuilder &baseArmor(const uint8_t armor_class);
        MonsterBuilder &
        critComponent(const uint8_t crit_chance,
                      const double  damage_multiplier,
                      std::initializer_list<EffectComponent *> crit_effects);
        MonsterBuilder &
        buffComponent(std::initializer_list<EffectComponent *> buffs);
        MonsterBuilder &coordinates(const uint16_t x, const uint16_t y);
        MonsterBuilder &tileComponent(const TileAppearance sprite);
        MonsterBuilder &experience(const uint8_t  level,
                                   const uint32_t current_experience);
        MonsterBuilder &inventory(std::initializer_list<Entity *> items);
        MonsterBuilder &weapon(Entity *const weapon);
        MonsterBuilder &armor(Entity *const armor);
        MonsterBuilder &rings(uint8_t                         max_rings,
                              std::initializer_list<Entity *> items);
    };

    bool rollChance(int chance);
    int  rollRandomFromRange(int min_value, int max_value);

    void addRandomArmor(Entity *const monster,
                        uint8_t       armor_chance,
                        uint8_t       rarity_modifier = 0);

    void addRandomWeapon(Entity *const monster,
                         uint8_t       weapon_chance,
                         uint8_t       rarity_modifier = 0);

    void addRandomRing(Entity *const monster,
                       uint8_t       ring_chance,
                       uint8_t       max_rings       = 5,
                       uint8_t       rarity_modifier = 0);

    void addRandomPotion(Entity *const monster,
                         uint8_t       potion_chance,
                         uint8_t       rarity_modifier = 0);

    void addRandomFoodRation(Entity *const monster,
                             uint8_t       food_chance,
                             uint8_t       rarity_modifier = 0);

    [[nodiscard]] Entity *generateBaseMonster();

    void generateRat(Entity *const monster);
    void generateBat(Entity *const monster);
    void generateGiantSpider(Entity *const monster);
    void generateViper(Entity *const monster);
    void generateKobold(Entity *const monster);
    void generateGoblin(Entity *const monster);
    void generateOrc(Entity *const monster);
    void generateTroll(Entity *const monster);
    void generateZombie(Entity *const monster);
    void generateDeathknight(Entity *const monster);
    void generateAssassin(Entity *const monster);
    void generateDemon(Entity *const monster);
    void generateVampire(Entity *const monster);
    void generateDragon(Entity *const monster);
    void generateGiantVenusFlytrap(Entity *const monster);

    void placeMonster(Entity *const monster);

public:
    MonsterFactory(ItemFactory &item_factory, int &depth);

    std::list<Entity *> generateMonsters() noexcept(false);

    void assignMap(observer_ptr<std::vector<std::vector<Tile>>> map);
};

#endif /*MONSTERFACTORY_H*/
