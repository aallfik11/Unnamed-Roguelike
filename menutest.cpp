#include "src/game/UI/gamescreen.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/entitymanager.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
#include "src/game/systems/positionsystem.h"
#include <chrono>
#include <thread>
int main()
{
    // MainMenu      main_menu;
    std::random_device rd;
    std::mt19937       mt(rd());
    auto               map   = *CaveGenerator::generate(mt, 100, 50);
    int                depth = 0;
    ItemFactory        it_fac(map, depth);
    MonsterFactory     mon_fac(it_fac, map, depth);
    it_fac.generateItems();
    mon_fac.generateMonsters();

    EntityManager  entity_manager;
    PositionSystem pos_sys(map);

    entity_manager.readSystemMessages();
    entity_manager.updateData();

    pos_sys.readSystemMessages();
    pos_sys.updateData();
}
