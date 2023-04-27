#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include <cstdint>
#include <iostream>
#include <memory>

int main()
{
    // std::cout << "HELLO!";
    // std::unique_ptr<ItemComponent> item(new ItemComponent(ItemType::ARMOR,
    // 10, 1, Rarity::LEGENDARY, true)); std::cout << item.get() << '\n';
    // std::cin >> item.get();
    // std::cout << item.get() << '\n';
    Entity    a(EntityType::CREATURE);
    Entity    b(a);
    Entity    c(a);
    Entity    d(a);
    Inventory inv;
    inv.inventory.emplace_back(&a);
    inv.inventory.emplace_back(&b);
    inv.inventory.emplace_back(&c);
    inv.inventory.emplace_back(&d);
    std::cout << &inv << '\n';
    std::cin >> &inv;
    auto x = (*System::system_messages_)[SystemType::ENTITY];
    for (auto y : x)
    {
        auto entity_list =
            std::any_cast<std::shared_ptr<std::list<uint32_t>>>(*(y.begin() + 2));
        for (auto message : *entity_list)
        {
            std::cout << message << " ";
        }
    }
}
