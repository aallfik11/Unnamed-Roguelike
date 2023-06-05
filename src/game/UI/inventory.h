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
#include "../systems/inventorysystem.h"
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
    bool             is_stats_open_;
    bool             is_desc_open_;
    InventorySystem &inv_sys_;

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
        auto  is_equipped    = item_component->equipped;

        MenuEntryOption option;
        option.animated_colors.foreground.enabled = true;
        option.animated_colors.background.enabled = true;
        // option.animated_colors.foreground.active   = col;
        // option.animated_colors.foreground.inactive = col;
        // option.animated_colors.background.active   =
        // ftxui::Color::DarkSlateGray1;
        // option.animated_colors.background.inactive = ftxui::Color::Black;

        option.transform = [is_equipped, col](EntryState state)
        {
            Element elem = text(state.label) | color(col);
            if (is_equipped == true)
            {
                elem = hbox({elem, text(" E") | color(Color::White)});
            }

            if (state.focused)
                elem = hbox({text("→ ") | color(Color::White), elem});

            return elem;
        };
        return option;
    }
    std::string getEffectDuration(Effect                        key,
                                  observer_ptr<EffectComponent> effect)
    {
        std::string duration{};
        if ((key & Effect::PERMANENT) == Effect::NONE)
        {
            duration =
                " - " + std::to_string(effect->effect_duration) + " turns";
        }
        return duration;
    }

    ftxui::Element getItemStats(observer_ptr<Entity> item)
    {
        using namespace ftxui;
        Elements stats;
        if (auto attack_rating = item->getComponent<WeaponComponent>())
        {
            stats.emplace_back(
                text("Attack Power: " + std::to_string(attack_rating->damage)));
        }

        if (auto armor_rating = item->getComponent<ArmorComponent>())
        {
            stats.emplace_back(text("Armor Class: " +
                                    std::to_string(armor_rating->armor_class)));
        }
        if (auto food_value = item->getComponent<HungerComponent>())
        {
            stats.emplace_back(
                text("Saturation: " + std::to_string(food_value->hunger)));
        }
        if (auto crit_component = item->getComponent<CritComponent>())
        {
            stats.emplace_back(text(
                "Crit Chance: " + std::to_string(crit_component->crit_chance) +
                "%"));
            stats.emplace_back(
                text("Crit Damage Multiplier: " +
                     std::to_string(crit_component->crit_multiplier)));
            if (crit_component->crit_effects->buffs.empty() != true)
            {
                stats.emplace_back(text("On-Crit Effects: "));
                for (auto &[key, effect] : crit_component->crit_effects->buffs)
                {
                    stats.emplace_back(
                        hbox({text(getEffectName(key) + " ") |
                                  color(getColorByEffect(key)),
                              text(std::to_string(effect->effect_strength) +
                                   getEffectDuration(key, effect.get()))}));
                }
            }
        }
        if (auto buffs = item->getComponent<BuffComponent>())
        {
            if (buffs->buffs.empty() != true)
            {
                stats.emplace_back(text("Effects: "));
                for (auto &[key, effect] : buffs->buffs)
                {
                    // auto key_clean = key & ~(Effect::APPLIED |
                    // Effect::PERMANENT | Effect::APPLY_ONCE);
                    stats.emplace_back(
                        hbox({text(getEffectName(key) + " ") |
                                  color(getColorByEffect(key)),
                              text(std::to_string(effect->effect_strength) +
                                   getEffectDuration(key, effect.get()))}));
                }
            }
        }
        return vbox(stats);
    }

public:
    InventoryUI(InventorySystem &inv_sys) : inv_sys_{inv_sys}
    {
        is_desc_open_  = false;
        is_stats_open_ = false;
    }
    void render(observer_ptr<Entity> player, ftxui::ScreenInteractive &scr)
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

        // auto scr           = ScreenInteractive::Fullscreen();
        auto inv_container = Container::Vertical(items, &inventory_index);

        auto renderer      = Renderer(
            [&]
            {
                auto item = *std::next(inventory.begin(), inventory_index);
                if (is_stats_open_ == true)
                {
                    return dbox(
                        {inv_container->Render(),
                         window(text(item->getComponent<Name>()->name) |
                                    color(getColorByRarity(
                                        item->getComponent<ItemComponent>()
                                            ->rarity)) |
                                    center,
                                getItemStats(item)) |
                             clear_under | center | flex_shrink});
                }
                if (is_desc_open_ == true)
                {
                    return dbox(
                        {inv_container->Render(),
                         window(text(item->getComponent<Name>()->name) |
                                    color(getColorByRarity(
                                        item->getComponent<ItemComponent>()
                                            ->rarity)) |
                                    center,
                                paragraphAlignJustify(
                                    item->getComponent<Description>()
                                        ->description)) |
                             clear_under | center | flex_shrink});
                }
                return inv_container->Render();
            });

        auto key_handler = CatchEvent(
            renderer,
            [&](Event event)
            {
                if (event == Event::Return)
                {
                    // System::sendSystemMessage(
                    //     SystemType::INVENTORY,
                    //     {std::make_any<SystemAction::INVENTORY>(
                    //          SystemAction::INVENTORY::USE),
                    //      std::make_any<observer_ptr<Entity>>(player),
                    //      std::make_any<uint32_t>(static_cast<uint32_t>(
                    //          inventory_index))}); // bad idea for now, can
                    //          lead
                    //                               // to exploits
                    inv_sys_.useItem(player, inventory_index);

                    return true;
                }
                if (event == Event::Escape)
                {
                    scr.Exit();
                    return true;
                }
                if (event == Event::Character('s') ||
                    event == Event::Character('S'))
                {
                    is_desc_open_  = false;
                    is_stats_open_ = (is_stats_open_) ? false : true;
                    return true;
                }
                if (event == Event::Character('d') ||
                    event == Event::Character('D'))
                {
                    is_desc_open_  = (is_desc_open_) ? false : true;
                    is_stats_open_ = false;
                    return true;
                }
                if (event == Event::Character('r') ||
                    event == Event::Character('R'))
                {
                    auto inv_size = inventory.size();
                    inv_sys_.dropFromInventory(player, inventory_index);
                    if (inventory.size() != inv_size)
                    {
                        if (inventory_index != 0)
                        {
                            --inventory_index;
                        }
                        else
                            ++inventory_index;
                    }
                    return true;
                }

                return inv_container->OnEvent(event);
            });
        scr.Loop(key_handler);
    }
};

#endif /*INVENTORY_UI_H*/
