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
    std::vector<std::vector<Tile>> &map_;
    int                            &current_depth_;
    std::list<Monsters>             low_level_monsters_;
    std::list<Monsters>             medium_level_monsters_;
    std::list<Monsters>             high_level_monsters_;
    std::random_device              rd_;
    std::mt19937                    mt_engine_;
    std::unordered_map<Monsters, std::function<Entity *(MonsterFactory *)>>
        monster_generators_;

    Entity *generateRat()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Rat")});
        return monster;
    }
    Entity *generateBat()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Bat")});
        return monster;
    }
    Entity *generateGiantSpider()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {new Name("Giant Spider")});
        return monster;
    }
    Entity *generateViper()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateKobold()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateGoblin()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateOrc()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateTroll()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateZombie()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateDeathknight()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateAssassin()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateDemon()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateVampire()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateDragon()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }
    Entity *generateGiantVenusFlytrap()
    {
        Entity *monster = new Entity(EntityType::CREATURE, {});
        return monster;
    }

public:
    MonsterFactory(std::vector<std::vector<Tile>> &map, int &depth)
        : map_{map}, current_depth_{depth}
    {
        mt_engine_                         = std::mt19937(rd_());
        monster_generators_[Monsters::RAT] = &MonsterFactory::generateRat;
        monster_generators_[Monsters::BAT] = &MonsterFactory::generateBat;
        monster_generators_[Monsters::GIANT_SPIDER] =
            &MonsterFactory::generateGiantSpider;
        monster_generators_[Monsters::VIPER]  = &MonsterFactory::generateViper;
        monster_generators_[Monsters::KOBOLD] = &MonsterFactory::generateKobold;
        monster_generators_[Monsters::GOBLIN] = &MonsterFactory::generateGoblin;
        monster_generators_[Monsters::ORC]    = &MonsterFactory::generateOrc;
        monster_generators_[Monsters::TROLL]  = &MonsterFactory::generateTroll;
        monster_generators_[Monsters::ZOMBIE] = &MonsterFactory::generateZombie;
        monster_generators_[Monsters::DEATHKNIGHT] =
            &MonsterFactory::generateDeathknight;
        monster_generators_[Monsters::ASSASSIN] =
            &MonsterFactory::generateAssassin;
        monster_generators_[Monsters::DEMON] = &MonsterFactory::generateDemon;
        monster_generators_[Monsters::VAMPIRE] =
            &MonsterFactory::generateVampire;
        monster_generators_[Monsters::DRAGON] = &MonsterFactory::generateDragon;
        monster_generators_[Monsters::GIANT_VENUS_FLYTRAP] =
            &MonsterFactory::generateGiantVenusFlytrap;
    }

    std::list<Entity *> generateMonsters()
    {
        std::list<Entity *> monsters;

        return monsters;
    }
};

#endif /*MONSTERFACTORY_H*/
