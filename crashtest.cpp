
#include "src/game/UI/gamescreen.h"
#include "src/game/UI/inventory.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/aisystem.h"
#include "src/game/systems/attacksystem.h"
#include "src/game/systems/effectsystem.h"
#include "src/game/systems/entitymanager.h"
#include "src/game/systems/experiencesystem.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/healthsystem.h"
#include "src/game/systems/inventorysystem.h"
#include "src/game/systems/lineofsightsystem.h"
#include "src/game/systems/mapmanager.h"
#include "src/game/systems/playerinputsystem.h"
#include "src/game/systems/positionsystem.h"
#include <cstdint>
#include <random>

void updateSystemData(System &system)
{
    system.readSystemMessages();
    system.updateData();
    system.clearSystemMessages();
}

void updateSystemData(std::initializer_list<System *> systems)
{
    for (auto &system : systems)
    {
        updateSystemData(*system);
    }
}

int main()
{
    int                depth = 0;
    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map = *CaveGenerator::generate(mt, 100, 50);
    EntityManager      entity_manager;
    ItemFactory        it_fac(map, depth);
    MonsterFactory     monster_fac(it_fac, map, depth);

    entity_manager.createEntity(EntityType::PLAYER,
                                {new WeaponSlot,
                                 new WeaponComponent(3),
                                 new ArmorSlot,
                                 new ArmorComponent,
                                 new AmuletSlot,
                                 new CritComponent,
                                 new ExperienceComponent,
                                 new BuffComponent,
                                 new TileComponent(TileAppearance::PLAYER),
                                 new Coordinates,
                                 new HungerComponent,
                                 new Health(10),
                                 new Inventory});

    InventorySystem inv_sys;
    HealthSystem    health_sys;
    AttackSystem    attack_sys;
    PositionSystem  pos_sys(map);
    auto            goblin = monster_fac.generateBaseMonster();
    monster_fac.generateGoblin(goblin);
    monster_fac.placeMonster(goblin);
    goblin->getComponent<ArmorSlot>()->armor_item = nullptr;
    updateSystemData(entity_manager);
    auto player = entity_manager.getEntity(1);

    while (goblin->getComponent<Health>()->alive == true)
    {
        attack_sys.attack(player, goblin);
        updateSystemData(health_sys);
    }
    int  *ptr    = &depth;
    auto &ref    = ptr;
    auto  any    = std::make_any<int *>(ref);
    auto  casted = std::any_cast<int *>(any);
    std::cout << *casted << "\n";

    updateSystemData(inv_sys);
    // updateSystemData(pos_sys);
    std::cout << "NO CRASH :3";
}
