#include "inventory.h"
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
#include <algorithm>
#include <any>
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <list>
#include <string>

inline ftxui::Color InventoryUI::getColorByRarity(Rarity rarity) const
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

inline ftxui::Color InventoryUI::getColorByEffect(Effect effect) const
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

inline std::string InventoryUI::getEffectName(Effect effect) const
{
    auto effect_cpy = effect;
    effect_cpy &= ~(Effect::APPLIED | Effect::PERMANENT | Effect::APPLY_ONCE);
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

ftxui::MenuEntryOption InventoryUI::getItemMenuEntry(observer_ptr<Entity> item)
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

    option.transform = [item_component, col](EntryState state)
    {
        Element elem = text(state.label) | color(col);
        if (item_component->equipped == true)
        {
            elem = hbox({elem, text(" E") | color(Color::White)});
        }

        if ((item_component->type & ItemType::STACKABLE) != ItemType::NONE)
        {
            elem = hbox({elem,
                         text(" " + std::to_string(item_component->stack)) |
                             color(Color::White)});
        }

        if (state.focused)
            elem = hbox({text("→ ") | color(Color::White), elem});

        return elem;
    };
    return option;
}
std::string InventoryUI::getEffectDuration(Effect                        key,
                                           observer_ptr<EffectComponent> effect)
{
    std::string duration{};
    if ((key & Effect::PERMANENT) == Effect::NONE)
    {
        duration = " - " + std::to_string(effect->effect_duration) + " turns";
    }
    return duration;
}

ftxui::Element InventoryUI::getItemStats(observer_ptr<Entity> item)
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
        stats.emplace_back(
            text("Armor Class: " + std::to_string(armor_rating->armor_class)));
    }
    if (auto food_value = item->getComponent<HungerComponent>())
    {
        stats.emplace_back(
            text("Saturation: " + std::to_string(food_value->hunger)));
    }
    if (auto crit_component = item->getComponent<CritComponent>())
    {
        stats.emplace_back(
            text("Crit Chance: " + std::to_string(crit_component->crit_chance) +
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
void InventoryUI::fixIndex(int &index, const std::size_t inventory_size)
{
    index = std::max(0, std::min(static_cast<int>(inventory_size - 1), index));
    return;
}

InventoryUI::InventoryUI(InventorySystem &inv_sys) : inv_sys_{inv_sys}
{
    is_desc_open_  = false;
    is_stats_open_ = false;
}
void InventoryUI::render(observer_ptr<Entity>      player,
                         ftxui::ScreenInteractive &scr)
{
    using namespace ftxui;
    int              inventory_index = 0;
    ftxui::Component inv_container;
    auto             flexbox_config = FlexboxConfig();
    flexbox_config.Set(FlexboxConfig::JustifyContent::SpaceEvenly);
    auto controls =
        flexbox({text("ESC - Exit Inventory Screen") | borderRounded,
                 text("ENTER - Equip/Use Item") | borderRounded,
                 text("D - Item Description") | borderRounded,
                 text("S - Item Statistics") | borderRounded},
                flexbox_config) |
        hcenter | flex_shrink;

    // Components items;
    inv_container = Container::Vertical(Components(), &inventory_index);
    auto renderer = Renderer(
        [&]
        {
            auto &inventory = player->getComponent<Inventory>()->inventory;
            inv_container->DetachAllChildren();
            // items.clear();
            if (inventory.empty())
            {
                inv_container->Add(MenuEntry(
                    "Inventory empty, try picking some items up first"));
                return inv_container->Render();
            }
            for (auto &item : inventory)
            {
                inv_container->Add(MenuEntry(item->getComponent<Name>()->name,
                                             getItemMenuEntry(item)));
            }

            auto item = *std::next(inventory.begin(), inventory_index);
            if (is_stats_open_ == true)
            {
                return vbox(
                    {dbox({inv_container->Render() | vscroll_indicator | frame,
                           window(text(item->getComponent<Name>()->name) |
                                      color(getColorByRarity(
                                          item->getComponent<ItemComponent>()
                                              ->rarity)) |
                                      center,
                                  getItemStats(item)) |
                               clear_under | center | flex_shrink}) |
                         flex_grow,
                     separatorLight(),
                     controls});
            }
            if (is_desc_open_ == true)
            {
                return vbox(
                    {dbox({inv_container->Render() | vscroll_indicator | frame,
                           window(text(item->getComponent<Name>()->name) |
                                      color(getColorByRarity(
                                          item->getComponent<ItemComponent>()
                                              ->rarity)) |
                                      center,
                                  paragraphAlignJustify(
                                      item->getComponent<Description>()
                                          ->description)) |
                               clear_under | center | flex_shrink}) |
                         flex_grow,
                     separatorLight(),
                     controls});
            }
            return vbox({inv_container->Render() | vscroll_indicator | frame |
                             flex_grow,
                         separatorLight(),
                         controls});
        });

    auto key_handler = CatchEvent(
        renderer,
        [&](Event event)
        {
            if (event == Event::Return)
            {
                inv_sys_.useItem(player, inventory_index);
            }
            if (event == Event::Escape)
            {
                scr.Exit();
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
                inv_sys_.dropFromInventory(player, inventory_index);
            }

            fixIndex(inventory_index,
                     player->getComponent<Inventory>()->inventory.size());
            return inv_container->OnEvent(event);
        });
    scr.Loop(key_handler);
}
