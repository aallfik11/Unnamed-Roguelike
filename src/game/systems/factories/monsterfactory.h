#ifndef MONSTERFACTORY_H
#define MONSTERFACTORY_H
#include "../../components/components_all.h"
#include "../../monsters.h"
#include "../../system.h"
#include "../../tile.h"
#include "itemfactory.h"
#include <functional>
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

    std::unordered_map<Monsters,
                       std::function<void(MonsterFactory *, Entity *const)>>
        monster_generators_;

    class MonsterBuilder
    {
        
    };

    Entity *generateBaseMonster()
    {
        return new Entity(EntityType::CREATURE,
                          {new Name(),
                           new AIComponent(),
                           new LOSComponent(),
                           new Health(),
                           new WeaponComponent(),
                           new ArmorComponent(),
                           new CritComponent(),
                           new BuffComponent(),
                           new Coordinates(),
                           new TileComponent(),
                           new ExperienceComponent});
    }

    void makeHumanoid(Entity *entity)
    {
        entity->addComponent(new Inventory);
        entity->addComponent(new ArmorSlot);
        entity->addComponent(new WeaponSlot);
        entity->addComponent(new AmuletSlot);
    }

    void setHealth(Entity *const monster, uint16_t max_hp)
    {
        auto hp                   = monster->getComponent<Health>();
        hp->max_health_points     = max_hp;
        hp->current_health_points = max_hp;
    }

    void generateRat(Entity *const monster)
    {
        monster->getComponent<Name>()->name = "Rat";

        monster->getComponent<AIComponent>()->ai_type =
            AIType::AI_MONSTER_COWARDLY;

        monster->getComponent<LOSComponent>()->seeing_distance = 25;

        auto hp                   = monster->getComponent<Health>();
        hp->max_health_points     = 5;
        hp->current_health_points = 5;

        monster->getComponent<WeaponComponent>()->damage = 2;

        auto crit             = monster->getComponent<CritComponent>();
        crit->crit_chance     = 1;
        crit->crit_multiplier = 1.0l;
        crit->crit_effects->buffs[Effect::POISON] =
            std::make_unique<EffectComponent>(Effect::POISON, 1, 3);

        monster->getComponent<TileComponent>()->sprite = TileAppearance::RAT;
    }
    void generateBat(Entity *const monster)
    {

        monster->getComponent<Name>()->name = "Bat";
        monster->getComponent<AIComponent>()->ai_type =
            AIType::AI_MONSTER_COWARDLY;
        monster->getComponent<LOSComponent>()->seeing_distance = 40;

        auto hp               = monster->getComponent<Health>();
        hp->max_health_points = 5;
    }
    void generateGiantSpider(Entity *const monster) {}
    void generateViper(Entity *const monster) {}
    void generateKobold(Entity *const monster) { makeHumanoid(monster); }
    void generateGoblin(Entity *const monster) { makeHumanoid(monster); }
    void generateOrc(Entity *const monster) { makeHumanoid(monster); }
    void generateTroll(Entity *const monster) { makeHumanoid(monster); }
    void generateZombie(Entity *const monster) { makeHumanoid(monster); }
    void generateDeathknight(Entity *const monster) { makeHumanoid(monster); }
    void generateAssassin(Entity *const monster) { makeHumanoid(monster); }
    void generateDemon(Entity *const monster) { makeHumanoid(monster); }
    void generateVampire(Entity *const monster) { makeHumanoid(monster); }
    void generateDragon(Entity *const monster) {}
    void generateGiantVenusFlytrap(Entity *const monster) {}

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

        return monsters;
    }
};

#endif /*MONSTERFACTORY_H*/
