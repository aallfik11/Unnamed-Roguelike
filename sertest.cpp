#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/observerptr.h"
#include "src/game/system.h"
#include "src/game/systems/aisystem.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/inventorysystem.h"
#include "src/game/systems/lineofsightsystem.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/navmapmanager.h"
#include "src/game/systems/positionsystem.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

int main()
{
    int                             depth = 0;
    std::list<observer_ptr<System>> systems;
    MapManager                      map_manager(DebugMapGenerator::generate);
    map_manager.generate(100, 50);
    auto map = map_manager.getMap();

    EntityManager entity_manager;
    entity_manager.createEntity(EntityType::PLAYER,
                                {new Name("Player"), new Coordinates(10, 10)});

    NavMapManager navmap_manager(
        *map, entity_manager.getEntity(1)->getComponent<Coordinates>());

    ItemFactory    item_factory(*map, depth);
    MonsterFactory monster_factory(item_factory, *map, depth);

    AISystem     ai_system(*map, navmap_manager, entity_manager.getEntity(1));
    AttackSystem attack_system;
    EffectSystem effect_system;
    HealthSystem health_system;
    InventorySystem inventory_system;
    LOS_System      los_system(*map, entity_manager.getEntity(1));
    PositionSystem  pos_system(*map);

    systems.emplace_back(&entity_manager);
    systems.emplace_back(&ai_system);
    systems.emplace_back(&effect_system);
    systems.emplace_back(&los_system);
    systems.emplace_back(&pos_system);

    item_factory.generateItems();
    monster_factory.generateMonsters();
    // for (auto &system : systems)
    // {
    //     system->readSystemMessages();
    //     system->updateData();
    //     system->clearSystemMessages();
    // }
    entity_manager.readSystemMessages();
    entity_manager.updateData();
    entity_manager.clearSystemMessages();

    pos_system.readSystemMessages();
    pos_system.updateData();
    pos_system.clearSystemMessages();

    effect_system.readSystemMessages();
    effect_system.updateData();
    effect_system.clearSystemMessages();

    los_system.readSystemMessages();
    los_system.updateData();
    los_system.clearSystemMessages();

    ai_system.readSystemMessages();
    ai_system.updateData();
    ai_system.clearSystemMessages();

    std::ofstream f("systems.txt");
    for (auto &system : systems)
    {
        f << *system << std::endl;
    }
    f.close();

    std::ifstream ifstr(std::filesystem::current_path() / "systems.txt");
    std::string   placeholder{};
    ifstr >> placeholder;
    ifstr >> entity_manager;
    ifstr >> placeholder;
    ifstr >> ai_system;
    ifstr >> placeholder;
    ifstr >> effect_system;
    ifstr >> placeholder;
    ifstr >> los_system;
    ifstr >> placeholder;
    ifstr >> pos_system;
    entity_manager.readSystemMessages();
    entity_manager.updateData();
    entity_manager.clearSystemMessages();
    // for (auto &system : systems)
    // {
    //     ifstr >> placeholder >> *system;
    // }
    //
    f.open("systems2.txt");
    for (auto &system : systems)
    {
        f << *system << std::endl;
    }

    std::cout << "No Crash :3";
}
