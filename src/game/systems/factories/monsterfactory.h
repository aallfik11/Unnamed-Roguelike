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
        MonsterBuilder(Entity *const monster)
        {
            is_humanoid_   = false;
            this->monster_ = monster;
        }

        MonsterBuilder &start(Entity *const monster)
        {
            this->monster_     = monster;
            this->is_humanoid_ = false;
            return *this;
        }

        MonsterBuilder &humanoid()
        {
            monster_->addComponent(new Inventory);
            monster_->addComponent(new ArmorSlot);
            monster_->addComponent(new WeaponSlot);
            monster_->addComponent(new AmuletSlot);
            this->is_humanoid_ = true;
            return *this;
        }

        MonsterBuilder &name(const std::string &name)
        {
            monster_->getComponent<Name>()->name = name;
            return *this;
        }

        MonsterBuilder &ai(const AIType &ai_type)
        {
            monster_->getComponent<AIComponent>()->ai_type = ai_type;
            return *this;
        }

        MonsterBuilder &los(const uint16_t seeing_distance)
        {
            monster_->getComponent<LOSComponent>()->seeing_distance =
                seeing_distance;
            return *this;
        }

        MonsterBuilder &health(const uint16_t max_hp)
        {
            monster_->getComponent<Health>()->max_health_points     = max_hp;
            monster_->getComponent<Health>()->current_health_points = max_hp;
            return *this;
        }

        MonsterBuilder &baseDamage(const uint16_t damage)
        {
            monster_->getComponent<WeaponComponent>()->damage = damage;
            return *this;
        }

        MonsterBuilder &baseArmor(const uint8_t armor_class)
        {
            monster_->getComponent<ArmorComponent>()->armor_class = armor_class;
            return *this;
        }

        MonsterBuilder &
        critComponent(const uint8_t crit_chance,
                      const double  damage_multiplier,
                      std::initializer_list<EffectComponent *> crit_effects)
        {
            auto crit_component = monster_->getComponent<CritComponent>();
            crit_component->crit_chance     = crit_chance;
            crit_component->crit_multiplier = damage_multiplier;
            crit_component->crit_effects =
                std::make_unique<BuffComponent>(crit_effects);
            return *this;
        }

        MonsterBuilder &
        buffComponent(std::initializer_list<EffectComponent *> buffs)
        {
            auto buff_component = monster_->getComponent<BuffComponent>();
            for (auto &buff : buffs)
            {
                buff_component->buffs[(
                    buff->effect & ~(Effect::APPLIED | Effect::APPLY_ONCE))] =
                    std::unique_ptr<EffectComponent>(buff);
            }
            return *this;
        }

        MonsterBuilder &coordinates(const uint16_t x, const uint16_t y)
        {
            monster_->getComponent<Coordinates>()->x = x;
            monster_->getComponent<Coordinates>()->y = y;
            return *this;
        }

        MonsterBuilder &tileComponent(const TileAppearance sprite)
        {
            monster_->getComponent<TileComponent>()->sprite = sprite;
            return *this;
        }

        MonsterBuilder &experience(const uint8_t  level,
                                   const uint32_t current_experience)
        {
            monster_->getComponent<ExperienceComponent>()->level = level;
            monster_->getComponent<ExperienceComponent>()->current_experience =
                current_experience;
            return *this;
        }

        MonsterBuilder &inventory(std::initializer_list<Entity *> items)
        {
            if (is_humanoid_ == false &&
                monster_->hasComponent<Inventory>() == false)
            {
                monster_->addComponent(new Inventory);
            }
            auto inventory = monster_->getComponent<Inventory>();
            for (auto &item : items)
            {
                inventory->inventory.emplace_back(item);
            }
            return *this;
        }

        MonsterBuilder &weapon(Entity *const weapon)
        {
            if (is_humanoid_ == false)
            {
                monster_->addComponent(new WeaponSlot);
                if (monster_->hasComponent<Inventory>() == false)
                {
                    monster_->addComponent(new Inventory);
                }
            }
            monster_->getComponent<WeaponSlot>()->weapon_item = weapon;
            monster_->getComponent<Inventory>()->inventory.emplace_back(weapon);
            weapon->getComponent<ItemComponent>()->equipped = true;
            return *this;
        }

        MonsterBuilder &armor(Entity *const armor)
        {
            if (is_humanoid_ == false)
            {
                monster_->addComponent(new ArmorSlot);
                if (monster_->hasComponent<Inventory>() == false)
                {
                    monster_->addComponent(new Inventory);
                }
            }
            monster_->getComponent<ArmorSlot>()->armor_item = armor;
            monster_->getComponent<Inventory>()->inventory.emplace_back(armor);
            armor->getComponent<ItemComponent>()->equipped = true;
            return *this;
        }

        MonsterBuilder &rings(uint8_t                         max_rings,
                              std::initializer_list<Entity *> items)
        {
            if (is_humanoid_ == false)
            {
                monster_->addComponent(new AmuletSlot);
                if (monster_->hasComponent<Inventory>() == false)
                {
                    monster_->addComponent(new Inventory);
                }
            }
            auto amulet_slot       = monster_->getComponent<AmuletSlot>();
            auto inv_ptr           = monster_->getComponent<Inventory>();
            amulet_slot->max_slots = max_rings;
            int i                  = 0;
            for (auto &item : items)
            {
                if (i >= max_rings)
                    break;

                amulet_slot->amulet_slots.insert(item);
                amulet_slot->amount_equipped += 1;
                inv_ptr->inventory.emplace_back(item);
                item->getComponent<ItemComponent>()->equipped = true;
                ++i;
            }
            return *this;
        }
    };

    bool rollChance(int chance)
    {
        if (chance <= 0)
            return false;
        if (chance >= 100)
            return true;
        static std::uniform_int_distribution<> roll(1, 100);
        return (chance <= roll(mt_engine_));
    }

    int rollRandomFromRange(int min_value, int max_value)
    {
        if (min_value > max_value)
        {
            int temp  = min_value;
            min_value = max_value;
            max_value = temp;
        }
        std::uniform_int_distribution<> roll(min_value, max_value);
        return roll(mt_engine_);
    }

    void addRandomArmor(Entity *const monster,
                        uint8_t       armor_chance,
                        uint8_t       rarity_modifier = 0)
    {
        std::uniform_int_distribution<uint8_t> chance_distro(1, 100);
        if (armor_chance + current_depth_ <= chance_distro(mt_engine_))
        {
            MonsterBuilder builder(monster);
            builder.armor(item_factory_.generateArmor(rarity_modifier));
        }
    }

    void addRandomWeapon(Entity *const monster,
                         uint8_t       weapon_chance,
                         uint8_t       rarity_modifier = 0)
    {
        std::uniform_int_distribution<uint8_t> chance_distro(1, 100);
        if (weapon_chance + current_depth_ <= chance_distro(mt_engine_))
        {
            MonsterBuilder builder(monster);
            builder.weapon(item_factory_.generateWeapon(rarity_modifier));
        }
    }

    void addRandomRing(Entity *const monster,
                       uint8_t       ring_chance,
                       uint8_t       max_rings       = 5,
                       uint8_t       rarity_modifier = 0)
    {
        std::uniform_int_distribution<uint8_t> chance_distro(1, 100);
        if (ring_chance + current_depth_ <= chance_distro(mt_engine_))
        {
            MonsterBuilder builder(monster);
            builder.rings(max_rings,
                          {item_factory_.generateRing(rarity_modifier)});
        }
    }

    void addRandomPotion(Entity *const monster,
                         uint8_t       potion_chance,
                         uint8_t       rarity_modifier = 0)
    {
        std::uniform_int_distribution<uint8_t> chance_distro(1, 100);
        if (potion_chance + current_depth_ <= chance_distro(mt_engine_))
        {
            MonsterBuilder builder(monster);
            builder.inventory({item_factory_.generatePotion(rarity_modifier)});
        }
    }

    void addRandomFoodRation(Entity *const monster,
                             uint8_t       food_chance,
                             uint8_t       rarity_modifier = 0)
    {

        std::uniform_int_distribution<uint8_t> chance_distro(1, 100);
        if (food_chance + current_depth_ <= chance_distro(mt_engine_))
        {
            MonsterBuilder builder(monster);
            builder.inventory(
                {item_factory_.generateFoodRation(rarity_modifier)});
        }
    }

    [[nodiscard]] Entity *generateBaseMonster()
    {
        return new Entity(EntityType::CREATURE,
                          {new Name,
                           new AIComponent,
                           new LOSComponent,
                           new NavMapComponent,
                           new Health,
                           new WeaponComponent,
                           new ArmorComponent,
                           new CritComponent,
                           new BuffComponent,
                           new Coordinates,
                           new TileComponent,
                           new ExperienceComponent});
    }

    void generateRat(Entity *const monster)
    {

        MonsterBuilder builder(monster);
        builder.name("Rat")
            .ai(AIType::AI_MONSTER_COWARDLY)
            .los(25)
            .health(5)
            .baseDamage(2)
            .critComponent(1, 1.0l, {new EffectComponent(Effect::POISON, 1, 3)})
            .tileComponent(TileAppearance::RAT)
            .experience(1, 25);
    }
    void generateBat(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Bat")
            .ai(AIType::AI_MONSTER_AGGRESSIVE)
            .los(40)
            .health(5)
            .baseDamage(3)
            .critComponent(1, 1.0l, {new EffectComponent(Effect::BLEED, 1, 2)})
            .tileComponent(TileAppearance::BAT)
            .experience(1, 30);
    }
    void generateGiantSpider(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Giant Spider")
            .ai(AIType::AI_MONSTER_DEFAULT)
            .los(15)
            .health(8)
            .baseDamage(4)
            .baseArmor(15)
            .critComponent(3,
                           1.0l,
                           {new EffectComponent(Effect::POISON, 1, 2),
                            new EffectComponent(Effect::BLIND, 1, 2)})
            .tileComponent(TileAppearance::GIANT_SPIDER)
            .experience(1, 40);
    }
    void generateViper(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Viper")
            .ai(AIType::AI_MONSTER_AGGRESSIVE)
            .los(15)
            .health(4)
            .baseDamage(1)
            .baseArmor(25)
            .critComponent(
                25, 1.0l, {new EffectComponent(Effect::POISON, 1, 4)})
            .tileComponent(TileAppearance::VIPER)
            .experience(1, 40);
    }
    void generateKobold(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Kobold")
            .ai(AIType::AI_MONSTER_DEFAULT)
            .los(20)
            .health(16)
            .baseDamage(3)
            .baseArmor(25)
            .critComponent(5, 1.5l, {new EffectComponent(Effect::BLEED, 2, 2)})
            .tileComponent(TileAppearance::KOBOLD)
            .experience(1, 100);

        addRandomWeapon(monster, 60);
        addRandomArmor(monster, 10);
        addRandomRing(monster, 5);
        addRandomPotion(monster, 15);
        addRandomFoodRation(monster, 30);
    }
    void generateGoblin(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Goblin")
            .ai(AIType::AI_MONSTER_DEFAULT)
            .los(20)
            .health(7)
            .baseDamage(2)
            .baseArmor(15)
            .critComponent(5, 1.5l, {})
            .tileComponent(TileAppearance::GOBLIN)
            .experience(1, 45);

        addRandomArmor(monster, 25);
        addRandomWeapon(monster, 75);
        addRandomPotion(monster, 5);
        addRandomFoodRation(monster, 20);
    }
    void generateOrc(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Orc")
            .ai(AIType::AI_MONSTER_AGGRESSIVE)
            .los(20)
            .health(20)
            .baseDamage(4)
            .baseArmor(15)
            .critComponent(15, 2.0l, {})
            .tileComponent(TileAppearance::ORC)
            .experience(1, 150);

        addRandomWeapon(monster, 90);
        addRandomArmor(monster, 75);
        addRandomPotion(monster, 5);
        addRandomFoodRation(monster, 60);
    }
    void generateTroll(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Troll")
            .ai(AIType::AI_MONSTER_BERSERK)
            .los(20)
            .health(40)
            .baseDamage(15)
            .baseArmor(30)
            .critComponent(15, 2.0l, {})
            .tileComponent(TileAppearance::TROLL)
            .experience(1, 400);

        addRandomFoodRation(monster, 30);
    }
    void generateZombie(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Zombie")
            .ai(AIType::AI_MONSTER_BERSERK)
            .los(20)
            .health(25)
            .baseDamage(2)
            .baseArmor(15)
            .critComponent(
                33, 1.5, {new EffectComponent(Effect::POISON, 1, 10)})
            .tileComponent(TileAppearance::ZOMBIE)
            .experience(1, 150);

        addRandomArmor(monster, 33);
        addRandomRing(monster, 5);
    }
    void generateDeathknight(Entity *const monster)
    {
        ItemBuilder sword;
        ItemBuilder armor;

        sword.itemComponent(ItemType::WEAPON, 1, 1, Rarity::LEGENDARY)
            .name("Deathknight Sword")
            .description(
                "A black sword, carried by the deathknights. Imbued with dark "
                "magicks capable of blinding the foe on a critical strike")
            .damage(rollRandomFromRange(30, 35))
            .crit(rollRandomFromRange(25, 35),
                  2.0l,
                  {new EffectComponent(Effect::BLIND, 1, 3)});

        armor.itemComponent(ItemType::ARMOR, 1, 1, Rarity::LEGENDARY)
            .name("Deathknight Armor")
            .description(
                "A black set of armor, Acting as a container for the knight's "
                "soul. It doesn't seem to possess any other magical "
                "properties, but it still offers solid protection")
            .armor(rollRandomFromRange(45, 55));

        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Deathknight")
            .ai(AIType::AI_MONSTER_BERSERK)
            .los(15)
            .health(30)
            .baseDamage(10)
            .baseArmor(30)
            .weapon(sword.build())
            .armor(armor.build())
            .tileComponent(TileAppearance::DEATHKNIGHT)
            .experience(1, 500);

        addRandomRing(monster, 35);
    }
    void generateAssassin(Entity *const monster)
    {
        ItemBuilder item_builder;

        item_builder.itemComponent(ItemType::WEAPON, 1, 1, Rarity::EPIC)
            .name("Assassin's Dagger")
            .description("A small, pointy blade, wielded by the assassins. "
                         "Capable of landing devastating critical strikes")
            .damage(5)
            .crit(40, 3.0l, {new EffectComponent(Effect::BLEED, 5, 3)});

        MonsterBuilder builder(monster);
        builder.humanoid()
            .name("Assassin")
            .ai(AIType::AI_MONSTER_DEFAULT)
            .los(25)
            .health(10)
            .baseDamage(5)
            .baseArmor(20)
            .critComponent(33, 3.0l, {})
            .weapon(item_builder.build())
            .tileComponent(TileAppearance::ASSASSIN)
            .experience(1, 500);
    }
    void generateDemon(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Demon")
            .ai(AIType::AI_MONSTER_AGGRESSIVE)
            .los(20)
            .health(30)
            .baseDamage(8)
            .baseArmor(30)
            .critComponent(100,
                           1.0l,
                           {new EffectComponent(Effect::BLEED, 1, 10),
                            new EffectComponent(Effect::BLIND, 1, 2)})
            .tileComponent(TileAppearance::DEMON)
            .experience(1, 500);
    }
    void generateVampire(Entity *const monster)
    {
        ItemBuilder vampiric_sword;
        ItemBuilder vampiric_armor;

        vampiric_sword.itemComponent(ItemType::WEAPON, 1, 1, Rarity::LEGENDARY)
            .name("Vampiric Sword")
            .description(
                "A serrated sword that causes heavy bleeding. It is imbued "
                "with the magical power of strength and regeneration")
            .damage(rollRandomFromRange(10, 14))
            .buffs({new EffectComponent(Effect::STRENGTH | Effect::PERMANENT,
                                        rollRandomFromRange(8, 12)),
                    new EffectComponent(Effect::HEAL | Effect::PERMANENT, 2)})
            .crit(100,
                  1.0l,
                  {new EffectComponent(Effect::BLEED,
                                       rollRandomFromRange(2, 5),
                                       rollRandomFromRange(1, 2))});

        vampiric_armor.itemComponent(ItemType::ARMOR, 1, 1, Rarity::LEGENDARY)
            .name("Vampiric Armor")
            .description("Armor worn by vampires. Its magical power grants "
                         "regeneration to its wearer")
            .armor(rollRandomFromRange(30, 45))
            .buffs({new EffectComponent(Effect::HEAL | Effect::PERMANENT,
                                        rollRandomFromRange(2, 4))});

        MonsterBuilder builder(monster);
        builder.name("Vampire")
            .ai(AIType::AI_MONSTER_DEFAULT)
            .los(30)
            .health(30)
            .baseDamage(5)
            .baseArmor(20)
            .critComponent(10, 2.0l, {new EffectComponent(Effect::BLEED, 2, 2)})
            .tileComponent(TileAppearance::VAMPIRE)
            .weapon(vampiric_sword.build())
            .armor(vampiric_armor.build())
            .experience(1, 500);
    }
    void generateDragon(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Dragon")
            .ai(AIType::AI_MONSTER_AGGRESSIVE)
            .los(40)
            .health(50)
            .baseDamage(40)
            .baseArmor(60)
            .critComponent(15, 1.5l, {})
            .tileComponent(TileAppearance::DRAGON)
            .experience(1, 600);
    }
    void generateGiantVenusFlytrap(Entity *const monster)
    {
        MonsterBuilder builder(monster);
        builder.name("Giant Venus Flytrap")
            .ai(AIType::AI_MONSTER_BERSERK)
            .los(10)
            .health(20)
            .baseDamage(8)
            .baseArmor(15)
            .critComponent(5,
                           1.5l,
                           {new EffectComponent(Effect::BLEED, 1, 2),
                            new EffectComponent(Effect::POISON, 1, 3)})
            .tileComponent(TileAppearance::GIANT_VENUS_FLYTRAP)
            .experience(1, 130);

        addRandomFoodRation(monster, 25);
    }

    void placeMonster(Entity *const monster)
    {
        int                                     size_x = (*map_).size();
        int                                     size_y = (*map_)[0].size();
        std::uniform_int_distribution<uint16_t> rand_x(1, size_x - 2);
        std::uniform_int_distribution<uint16_t> rand_y(1, size_y - 2);

        auto x = rand_x(mt_engine_);
        auto y = rand_y(mt_engine_);
        while (((*map_)[x][y].type & (TileType::WALL | TileType::HAS_STAIRS |
                                      TileType::HAS_CREATURE)) !=
               TileType::NONE)
        {
            x = rand_x(mt_engine_);
            y = rand_y(mt_engine_);
        }
        auto coordinates    = monster->getComponent<Coordinates>();
        coordinates->x      = x;
        coordinates->y      = y;
        (*map_)[x][y].type |= TileType::HAS_CREATURE;
        System::sendSystemMessage(SystemType::POSITION,
                                  {std::make_any<SystemAction::POSITION>(
                                       SystemAction::POSITION::UPDATE),
                                   std::make_any<Entity *>(monster),
                                   std::make_any<uint16_t>(x),
                                   std::make_any<uint16_t>(y)});
    }

public:
    MonsterFactory(ItemFactory &item_factory, int &depth)
        : item_factory_{item_factory}, current_depth_{depth}
    {
        mt_engine_               = std::mt19937(rd_());
        monster_generators_[RAT] = &MonsterFactory::generateRat;
        monster_generators_[BAT] = &MonsterFactory::generateBat;
        monster_generators_[GIANT_SPIDER] =
            &MonsterFactory::generateGiantSpider;
        monster_generators_[VIPER]       = &MonsterFactory::generateViper;
        monster_generators_[KOBOLD]      = &MonsterFactory::generateKobold;
        monster_generators_[GOBLIN]      = &MonsterFactory::generateGoblin;
        monster_generators_[ORC]         = &MonsterFactory::generateOrc;
        monster_generators_[TROLL]       = &MonsterFactory::generateTroll;
        monster_generators_[ZOMBIE]      = &MonsterFactory::generateZombie;
        monster_generators_[DEATHKNIGHT] = &MonsterFactory::generateDeathknight;
        monster_generators_[ASSASSIN]    = &MonsterFactory::generateAssassin;
        monster_generators_[DEMON]       = &MonsterFactory::generateDemon;
        monster_generators_[VAMPIRE]     = &MonsterFactory::generateVampire;
        monster_generators_[DRAGON]      = &MonsterFactory::generateDragon;
        monster_generators_[GIANT_VENUS_FLYTRAP] =
            &MonsterFactory::generateGiantVenusFlytrap;

        low_level_monsters_ =
            std::vector<Monsters>({RAT, BAT, GIANT_SPIDER, VIPER, GOBLIN});
        med_level_monsters =
            std::vector<Monsters>({KOBOLD, GIANT_VENUS_FLYTRAP, ORC, ZOMBIE});
        high_level_monsters_ = std::vector<Monsters>(
            {TROLL, DEATHKNIGHT, ASSASSIN, DEMON, VAMPIRE, DRAGON});

        low_level_monster_amount_distro_ =
            std::uniform_int_distribution<>(0, 0);
        med_level_monster_amount_distro_ =
            std::uniform_int_distribution<>(0, 0);
        high_level_monster_amount_distro_ =
            std::uniform_int_distribution<>(0, 0);

        random_low_lvl_monster_ =
            std::uniform_int_distribution<>(0, low_level_monsters_.size() - 1);
        random_med_lvl_monster_ =
            std::uniform_int_distribution<>(0, med_level_monsters.size() - 1);
        random_high_lvl_monster_ =
            std::uniform_int_distribution<>(0, high_level_monsters_.size() - 1);
    }

    std::list<Entity *> generateMonsters() noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error(
                "Monster Factory: ERROR -> Map unassigned");
        monster_amount_distro_ =
            std::uniform_int_distribution<>(5, 5 + ((current_depth_ * 2) / 3));
        std::list<Entity *> monsters;

        if (current_depth_ < 9)
        {
            low_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(5, 5 + current_depth_);
        }
        else if (current_depth_ < 18)
        {
            low_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(0, current_depth_ / 3);
            med_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(5, 5 + current_depth_);
        }
        else if (current_depth_ < 27)
        {
            low_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(0, 0);
            med_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(0, current_depth_ / 3);
            high_level_monster_amount_distro_ =
                std::uniform_int_distribution<>(10, 10 + current_depth_);
        }

        int low_level_monster_amount =
            low_level_monster_amount_distro_(mt_engine_);
        int med_level_monster_amount =
            med_level_monster_amount_distro_(mt_engine_);
        int high_level_monster_amount =
            high_level_monster_amount_distro_(mt_engine_);

        int      roll{};
        Monsters monster_type{};

        for (int i = 0; i < low_level_monster_amount; ++i)
        {
            roll         = random_low_lvl_monster_(mt_engine_);
            monster_type = low_level_monsters_[roll];
            auto monster = generateBaseMonster();
            monster_generators_[monster_type](this, monster);
            monsters.emplace_back(monster);
        }
        for (int i = 0; i < med_level_monster_amount; ++i)
        {
            roll         = random_med_lvl_monster_(mt_engine_);
            monster_type = med_level_monsters[roll];
            auto monster = generateBaseMonster();
            monster_generators_[monster_type](this, monster);
            monsters.emplace_back(monster);
        }
        for (int i = 0; i < high_level_monster_amount; ++i)
        {
            roll         = random_high_lvl_monster_(mt_engine_);
            monster_type = high_level_monsters_[roll];
            auto monster = generateBaseMonster();
            monster_generators_[monster_type](this, monster);
            monsters.emplace_back(monster);
        }

        for (auto &monster : monsters)
        {
            placeMonster(monster);
            auto message = {
                std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::ADD),
                std::make_any<Entity *>(monster)};
            System::sendSystemMessage(SystemType::ENTITY, message);
            System::sendSystemMessage(
                SystemType::AI,
                {std::make_any<SystemAction::AI>(SystemAction::AI::ADD),
                 std::make_any<Entity *>(monster)});

            System::sendSystemMessage(
                SystemType::LINE_OF_SIGHT,
                {std::make_any<SystemAction::LINE_OF_SIGHT>(
                     SystemAction::LINE_OF_SIGHT::ADD),
                 std::make_any<Entity *>(monster)});

            System::sendSystemMessage(SystemType::EFFECT,
                                      {std::make_any<SystemAction::EFFECT>(
                                           SystemAction::EFFECT::ADD_ENTITY),
                                       std::make_any<Entity *>(monster)});
        }

        return monsters;
    }

    void assignMap(observer_ptr<std::vector<std::vector<Tile>>> map)
    {
        this->map_ = map;
    }
};

#endif /*MONSTERFACTORY_H*/
