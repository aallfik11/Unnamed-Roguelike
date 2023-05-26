#include "src/game/UI/gamescreen.h"
#include "src/game/UI/mainmenu.h"
#include "src/game/systems/factories/itemfactory.h"
#include "src/game/systems/factories/monsterfactory.h"
#include "src/game/systems/generators/cavegenerator.h"
#include "src/game/systems/generators/debugmapgenerator.h"
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

    // for (auto &message : (*System::system_messages_)[SystemType::ENTITY])
    // {
    //     auto                 message_it = message.begin();
    //     SystemAction::ENTITY action =
    //         std::any_cast<SystemAction::ENTITY>(*message_it);
    //     message_it++;
    //     switch (action)
    //     {
    //     case SystemAction::ENTITY::ADD:
    //     {
    //         auto entity = std::any_cast<observer_ptr<Entity>>(*message_it);
    //         if (entity->hasComponent<Coordinates>())
    //         {
    //             auto coord = entity->getComponent<Coordinates>();
    //             map[coord->x][coord->y].appearance =
    //                 entity->getComponent<TileComponent>()->sprite;
    //         }
    //         break;
    //     }
    //     }
    // }
    GameScreen scr;

    LaunchOptions options{};
    // main_menu.renderMainMenu(options);
    using namespace ftxui;
    // Elements sprites;
    // for (auto [key, appearance] : scr.appearances_)
    // {
    //     sprites.push_back(appearance);
    // }
    // std::cout << scr.appearances_.size();
    auto screen   = ScreenInteractive::FitComponent();
    // screen.Loop(
    //     Renderer([&] { return window(text("sprites"), vbox(sprites)); }));
    auto renderer = Renderer(
        [&]
        {
            Elements cols;
            for (int i = 0; i < 100; ++i)
            {
                Elements cells;
                for (int j = 0; j < 50; ++j)
                {
                    // if (map[i][j].appearance == TileAppearance::WALL)
                    // {
                    //     cells.push_back(text(" ") | bgcolor(Color::Grey0));
                    // }
                    // else
                        cells.push_back(scr.appearances_[map[i][j].appearance]);
                }
                cols.push_back(vbox(cells));
            }
            return hbox(cols);
        });
    screen.Loop(renderer);
}
