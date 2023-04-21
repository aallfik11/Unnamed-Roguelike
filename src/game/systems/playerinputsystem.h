#ifndef PLAYERCONTROLSYSTEM_H
#define PLAYERCONTROLSYSTEM_H

#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/description.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../entity.h"
#include "../itemtypes.h"
#include "../rarity.h"
#include "../system.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <list>
#include <memory>

class PlayerControlSystem : public System
{
    using EntityPtr = std::shared_ptr<Entity>;
    EntityPtr        player_;
    ftxui::Component inv_renderer_;
    ftxui::Component inv_input_handler_;

    inline void determineNextAction(const ftxui::Event &event)
    {

        using namespace ftxui;
        if (event.is_character())
        {
            // open inventory
            if (event == Event::Character("i") ||
                event == Event::Character("I"))
            {
            }

            // rest
            if (event == Event::Character("r") ||
                event == Event::Character("R"))
            {
                auto message = {std::make_any<EntityPtr>(player_),
                                std::make_any<uint16_t>(1),
                                std::make_any<SystemAction::HEALTH>(
                                    SystemAction::HEALTH::HEAL |
                                    SystemAction::HEALTH::CURRENT)};
                sendSystemMessage(SystemType::HEALTH, message);
            }

            // logs
            if (event == Event::Character("l") ||
                event == Event::Character("L"))
            {
            }

            // save
            if (event == Event::Character("s") ||
                event == Event::Character("S"))
            {
                /*SAVE SYSTEM GOES HERE*/
            }
        }
        if (event == Event::ArrowDown)
        {
        }
        if (event == Event::ArrowUp)
        {
        }
        if (event == Event::ArrowLeft)
        {
        }
        if (event == Event::ArrowRight)
        {
        }
    }

    inline ftxui::Color getColor(Rarity &rarity)
    {
        switch (rarity)
        {
        case COMMON:
            return ftxui::Color::GrayLight;
        case UNCOMMON:
            return ftxui::Color::Green3;
        case RARE:
            return ftxui::Color::Aquamarine1;
        case EPIC:
            return ftxui::Color::Purple;
        case LEGENDARY:
            return ftxui::Color::Gold1;
        default:
            return ftxui::Color::White;
        }
    }

    inline ftxui::MenuEntryOption
    getItemAppearance(const std::shared_ptr<ItemComponent> &item_component)
    {
        using namespace ftxui;
        Color col = getColor(item_component->rarity);

        MenuEntryOption option;
        option.animated_colors.foreground.enabled = true;
        option.animated_colors.background.enabled = true;
        // option.animated_colors.foreground.active   = col;
        // option.animated_colors.foreground.inactive = col;
        // option.animated_colors.background.active   =
        // ftxui::Color::DarkSlateGray1;
        // option.animated_colors.background.inactive = ftxui::Color::Black;

        option.transform = [&item_component, col](EntryState state)
        {
            Element elem = text(state.label) | color(col);
            if (item_component->equipped)
            {
                elem = hbox({elem, text(" E") | color(Color::White)});
            }

            if (state.focused)
                elem = hbox({text("→ ") | color(Color::White), elem});

            return elem;
        };
        return option;
    }

    void openInventory() {}

public:
    PlayerControlSystem(const EntityPtr        &player,
                        const ftxui::Component &main_screen)
    {
        player_ = player;

        using namespace ftxui;

        inv_renderer_ =
            Renderer(main_screen,
                     [&]
                     {
                         auto &player_inventory =
                             player_->getComponent<Inventory>()->inventory;

                         Components entries;
                         for (auto &item : player_inventory)
                         {
                             auto &name = item->getComponent<Name>()->name;
                             auto &description =
                                 item->getComponent<Description>()->description;
                             auto item_component =
                                 item->getComponent<ItemComponent>();
                             entries.push_back(MenuEntry(name, getItemAppearance(item_component)));
                         }

                         return dbox(main_screen->Render());
                     });
    }
    void updateData() override {}
    void readSystemMessages() override {}
    void clearSystemMessages() override {}
};

#endif /*PLAYERCONTROLSYSTEM_H*/
