// #include "src/game/components/components_all.h"
#include "src/game/components/armorslot.h"
#include "src/game/components/weaponslot.h"
#include "src/game/entity.h"
#include <iostream>
#include <memory>
// #include "src/game/systems/entitymanager.h"
// #include "src/game/components/components_all.h"

int main()
{
    // EntityManager entity_manager;
    // entity_manager.createEntity(
    //     EntityType::PLAYER,
    //     {
    //         // new WeaponSlot,
    //         // new WeaponComponent(200),
    //         // new ArmorSlot,
    //         // new ArmorComponent(100),
    //         // new AmuletSlot,
    //         // new CritComponent,
    //         // new ExperienceComponent,
    //         // new BuffComponent,
    //         // new TileComponent(TileAppearance::PLAYER),
    //         // new Coordinates,
    //         // new HungerComponent,
    //         // new Health(10),
    //         // new Inventory
    //     });
    // entity_manager.resetSystem();

    std::vector<Component *> components = {

        // new WeaponSlot,
        // new WeaponComponent(200),
        new ArmorSlot,
        // new ArmorComponent(100),
        // new AmuletSlot,
        // new CritComponent,
        // new ExperienceComponent,
        // new BuffComponent,
        // new TileComponent(TileAppearance::PLAYER),
        // new Coordinates,
        // new HungerComponent,
        // new Health(10),
        // new Inventory
    };

    std::unique_ptr<Entity> ent =
        std::make_unique<Entity>(EntityType::PLAYER, components);
    auto copy = std::make_unique<Entity>(*ent);
    std::cout << "nocrash";
}
