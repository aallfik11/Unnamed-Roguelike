#ifndef MONSTERFACTORY_H
#define MONSTERFACTORY_H
#include "../../components/components_all.h"
#include "../../monsters.h"
#include "../../system.h"
#include "../../tile.h"
#include "itemfactory.h"
#include <functional>
#include <list>
#include <random>
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

    std::vector<std::vector<Tile>> &map_;
    int                            &current_depth_;
    std::vector<Monsters>           low_level_monsters_;
    std::vector<Monsters>           med_level_monsters;
    std::vector<Monsters>           high_level_monsters_;
    std::random_device              rd_;
    std::mt19937                    mt_engine_;

    std::uniform_int_distribution<> monster_amount_distro_;
    std::uniform_int_distribution<> low_level_monster_amount_distro_;
    std::uniform_int_distribution<> med_level_monster_amount_distro_;
    std::uniform_int_distribution<> high_level_monster_amount_distro_;
    std::uniform_int_distribution<> random_low_lvl_monster_;
    std::uniform_int_distribution<> random_med_lvl_monster_;
    std::uniform_int_distribution<> random_high_lvl_monster_;

    std::unordered_map<Monsters, std::function<Entity *(MonsterFactory *)>>
        monster_generators_;

    Entity *generateRat()
    {
        Entity *monster = new Entity(
            EntityType::CREATURE,
            {new Name("Rat"),
             new Health(5),
             new ArmorComponent(),
             new CritComponent(1,
                               1.0l,
                               new BuffComponent({new EffectComponent(
                                   Effect::POISON, 1, 3)}))});
        return monster;
    }
    Entity *generateBat()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Bat")});
        return monster;
    }
    Entity *generateGiantSpider()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Giant Spider")});
        return monster;
    }
    Entity *generateViper()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Viper")});
        return monster;
    }
    Entity *generateKobold()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Kobold")});
        return monster;
    }
    Entity *generateGoblin()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Goblin")});
        return monster;
    }
    Entity *generateOrc()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Orc")});
        return monster;
    }
    Entity *generateTroll()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Troll")});
        return monster;
    }
    Entity *generateZombie()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Zombie")});
        return monster;
    }
    Entity *generateDeathknight()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Deathknight")});
        return monster;
    }
    Entity *generateAssassin()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Assassin")});
        return monster;
    }
    Entity *generateDemon()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Demon")});
        return monster;
    }
    Entity *generateVampire()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Vampire")});
        return monster;
    }
    Entity *generateDragon()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Dragon")});
        return monster;
    }
    Entity *generateGiantVenusFlytrap()
    {
        Entity *monster =
            new Entity(EntityType::CREATURE, {new Name("Giant Venus Flytrap")});
        return monster;
    }

public:
    MonsterFactory(std::vector<std::vector<Tile>> &map, int &depth)
        : map_{map}, current_depth_{depth}
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

    std::list<Entity *> generateMonsters()
    {
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
            monsters.emplace_back(monster_generators_[monster_type](this));
        }
        for (int i = 0; i < med_level_monster_amount; ++i)
        {
            roll         = random_med_lvl_monster_(mt_engine_);
            monster_type = med_level_monsters[roll];
            monsters.emplace_back(monster_generators_[monster_type](this));
        }
        for (int i = 0; i < high_level_monster_amount; ++i)
        {
            roll         = random_high_lvl_monster_(mt_engine_);
            monster_type = high_level_monsters_[roll];
            monsters.emplace_back(monster_generators_[monster_type](this));
        }

        return monsters;
    }
};

#endif /*MONSTERFACTORY_H*/
