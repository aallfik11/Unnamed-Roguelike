#include "src/game/components/components_all.h"
#include "src/game/entity.h"
#include "src/game/systems/inventorysystem.h"
#include <chrono>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <memory>

int main()
{

    using namespace ftxui;
    ftxui::Component a;

    int  index    = 0;
    auto cont     = Container::Vertical({MenuEntry("test")}, &index);
    auto renderer = Renderer(cont, [&] { return cont->Render(); });
    auto wrap     = CatchEvent(cont, [&](Event event) { return true; });

    auto scr = ScreenInteractive::Fullscreen();
    scr.Loop(wrap);
}