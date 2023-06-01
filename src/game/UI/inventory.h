#ifndef INVENTORY_UI_H
#define INVENTORY_UI_H
#include "../components/armorcomponent.h"
#include "../components/buffcomponent.h"
#include "../components/critcomponent.h"
#include "../components/description.h"
#include "../components/hungercomponent.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/weaponcomponent.h"
#include "../effect.h"
#include "../entity.h"
#include "../observerptr.h"
#include "../rarity.h"
#include "../system.h"
#include <any>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <list>
#include <string>

class InventoryUI
{
    inline ftxui::Color getColorByRarity(Rarity rarity) const
    {
        using namespace ftxui;

        switch (rarity)
        {
        case Rarity::COMMON:
            return Color::GrayLight;
        case Rarity::UNCOMMON:
            return Color::Green3;
        case Rarity::RARE:
            return Color::Cyan;
        case Rarity::EPIC:
            return Color::Purple;
        case Rarity::LEGENDARY:
            return Color::Gold1;
        default:
            return Color::White;
        }
    }

    inline ftxui::Color getColorByEffect(Effect effect) const
    {
        using namespace ftxui;
        effect &= ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
        switch (effect)
        {
        case Effect::HEAL:
            return Color::Pink1;
        case Effect::BLEED:
            return Color::Red;
        case Effect::BLIND:
            return Color::GrayLight;
        case Effect::POISON:
            return Color::Green;
        case Effect::IRONSKIN:
            return Color::Yellow;
        case Effect::STRENGTH:
            return Color::DarkRed;
        default:
            return Color::White;
        }
    }

    inline std::string getEffectName(Effect effect) const
    {
        auto effect_cpy = effect;
        effect_cpy &=
            ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
        std::string name = "";
        switch (effect_cpy)
        {
        case Effect::HEAL:
            name = "HEAL";
            break;
        case Effect::BLEED:
            name = "BLEED";
            break;
        case Effect::BLIND:
            name = "BLIND";
            break;
        case Effect::POISON:
            name = "POISON";
            break;
        case Effect::IRONSKIN:
            name = "IRONSKIN";
            break;
        case Effect::STRENGTH:
            name = "STRENGTH";
            break;
        default:
            name = "INVALID";
        }
        return name;
    }

    ftxui::MenuEntryOption getItemMenuEntry(observer_ptr<Entity> item)
    {
        using namespace ftxui;
        auto  item_component = item->getComponent<ItemComponent>();
        Color col            = getColorByRarity(item_component->rarity);

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

public:
    void render(observer_ptr<Entity> player)
    {
        auto &inventory = player->getComponent<Inventory>()->inventory;
        using namespace ftxui;
        int        inventory_index = 0;
        Components items;
        if (inventory.empty())
        {
            items.emplace_back(
                MenuEntry("Inventory empty, try picking some items up first"));
        }
        for (auto &item : inventory)
        {
            items.emplace_back(MenuEntry(item->getComponent<Name>()->name,
                                         getItemMenuEntry(item)));
        }
        auto scr           = ScreenInteractive::Fullscreen();
        auto inv_container = Container::Vertical(items, &inventory_index);
        auto key_handler   = CatchEvent(
            inv_container,
            [&](Event event)
            {
                if (event == Event::Return)
                {
                    System::sendSystemMessage(
                        SystemType::INVENTORY,
                        {std::make_any<SystemAction::INVENTORY>(
                             SystemAction::INVENTORY::USE),
                         std::make_any<observer_ptr<Entity>>(player),
                         std::make_any<uint32_t>(static_cast<uint32_t>(
                             inventory_index))}); // bad idea for now, can lead
                                                  // to exploits

                    return true;
                }
                if (event == Event::Escape)
                {

                    return true;
                }
                if (event == Event::Character('s') ||
                    event == Event::Character('S'))
                {
                    return true;
                }
                if (event == Event::Character('d') ||
                    event == Event::Character('D'))
                {
                    return true;
                }
                if (event == Event::Character('r') ||
                    event == Event::Character('R'))
                {
                    return true;
                }

                return inv_container->OnEvent(event);
            });
    }
};

#endif /*INVENTORY_UI_H*/
