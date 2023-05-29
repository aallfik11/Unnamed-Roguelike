#include "src/game/UI/gamescreen.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/effectsystem.h"
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

    auto weapon = it_fac.generateWeapon(50);
    auto armor  = it_fac.generateArmor(10);
    auto ring   = it_fac.generateRing(14);

    auto player = new Entity(EntityType::PLAYER,
                             {
                                 new WeaponComponent(19),
                                 new ArmorComponent(10),
                                 new WeaponSlot(weapon),
                                 new ArmorSlot(armor),
                                 new AmuletSlot,
                                 new Inventory,
                                 new BuffComponent({
                                     new EffectComponent(Effect::BLEED, 4, 10),
                                 }),
                                 new Health(10, 4),
                                 new HungerComponent(73, 100),
                                 new ExperienceComponent(1, 23),
                             });
    player->getComponent<AmuletSlot>()->amulet_slots.emplace(ring);
    entity_manager.addEntity(player);

    EffectSystem eff_sys;
    eff_sys.addEntity(player);
    eff_sys.addEffects(player, ring->getComponent<BuffComponent>());

    GameScreen game_screen(&map, player, pos_sys);
    auto       scr      = ftxui::ScreenInteractive::Fullscreen();
    auto       renderer = ftxui::Renderer(
        [&]
        {
            return ftxui::hbox({
                       game_screen.getSidebar(),
                       ftxui::separatorLight(),
                       game_screen.getMapBox(),
                   }) |
                   ftxui::borderRounded | ftxui::center | ftxui::flex_shrink;
        });

    auto eventhandler = ftxui::CatchEvent(renderer,
                                          [&](ftxui::Event event)
                                          {
                                              if (event == ftxui::Event::Escape)
                                              {
                                                  scr.Exit();
                                              }
                                              return true;
                                          });
    auto loop         = ftxui::Loop(&scr, eventhandler);
    loop.Run();
}
