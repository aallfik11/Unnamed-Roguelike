// #include "src/game/UI/gamescreen.h"
// #include "src/game/UI/inventory.h"
#include "src/game/UI/mainmenu.h"
// #include "src/game/systems/aisystem.h"
// #include "src/game/systems/attacksystem.h"
// #include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
// #include "src/game/systems/experiencesystem.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
// #include "src/game/systems/healthsystem.h"
// #include "src/game/systems/inventorysystem.h"
// #include "src/game/systems/lineofsightsystem.h"
#include "src/game/systems/mapmanager.h"
// #include "src/game/systems/playerinputsystem.h"
// #include "src/game/systems/positionsystem.h"
#include <cstdint>
#include <random>

int main()
{
    MainMenu           main_menu;
    LaunchOptions      option = LaunchOptions::NONE;
    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map = *CaveGenerator::generate(mt, 100, 50);
    // EntityManager      entity_manager;
    //
    // entity_manager.createEntity(EntityType::PLAYER,
    //                             {new WeaponSlot,
    //                              new WeaponComponent(3),
    //                              new ArmorSlot,
    //                              new ArmorComponent,
    //                              new AmuletSlot,
    //                              new CritComponent,
    //                              new ExperienceComponent,
    //                              new BuffComponent,
    //                              new TileComponent(TileAppearance::PLAYER),
    //                              new Coordinates,
    //                              new HungerComponent,
    //                              new Health(10),
    //                              new Inventory});

    int         depth = 0;
    ItemFactory it_fac(map, depth);
    MonsterFactory monster_fac(it_fac, map, depth);
    //
    it_fac.generateItems();
    monster_fac.generateMonsters();
    return 0;
}
