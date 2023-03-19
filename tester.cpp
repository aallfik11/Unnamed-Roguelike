#include "src/game/entity.h"
#include "src/game/component.h"
#include "src/game/components/testcomponent.h"
#include "src/game/components/testcomponent2.h"
#include "src/game/components/inventory.h"
#include <iostream>
#include <chrono>

int main()
{
    Entity ent;
    std::shared_ptr<Component> comp(new TestComponent2());
    ent.add_component(comp);
    ent.add_component(new TestComponent());
    ent.add_component(new Inventory());
    std::cout << std::boolalpha;
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << ent.has_component<Inventory>() << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto time_passed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    auto a = ent.get_component<TestComponent>();
    a->test = "MMM PUUU";
    std::cout << a->test << std::endl;

    std::cout << "time for finding component is: " << time_passed;
}
