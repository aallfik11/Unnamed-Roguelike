#include "build\_deps\ftxui-src\src\ftxui\component\container.cpp"
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

using namespace ftxui;

enum class PopupType : uint8_t
{
    NONE        = 0x00,
    DESCRIPTION = 0x01,
    STATS       = 0x02,
};

inline Color getColor(Rarity rarity)
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

inline constexpr PopupType operator^(PopupType p1, PopupType p2)
{
    return static_cast<PopupType>(static_cast<uint8_t>(p1) ^
                                  static_cast<uint8_t>(p2));
}

inline constexpr PopupType operator&(PopupType p1, PopupType p2)
{
    return static_cast<PopupType>(static_cast<uint8_t>(p1) &
                                  static_cast<uint8_t>(p2));
}

inline PopupType &operator^=(PopupType &p1, PopupType p2)
{
    p1 = p1 ^ p2;
    return p1;
}

namespace ftxui::Container
{
class InventoryContainer : public VerticalContainer
{

    std::shared_ptr<::Entity>                      entity_;
    std::list<std::shared_ptr<::Entity>>          &inventory_;
    std::list<std::shared_ptr<::Entity>>::iterator it_;
    PopupType                                      popup_;

protected:
    void MoveSelector(int dir) // for some reason I can't override this
    {
        for (int i  = *selector_ + dir; i >= 0 && i < (int)children_.size();
             i     += dir)
        {
            (dir > 0) ? ++it_ : --it_;
            if (children_[i]->Focusable())
            {
                *selector_ = i;
                return;
            }
        }
    }

    void containerCorrection()
    {
        children_.erase(children_.begin() + *selector_);

        int selector_cpy = *selector_;
        *selector_       = 0;
        it_              = inventory_.begin();

        for (size_t i = 0; i < (size_t)selector_cpy; i++)
        {
            MoveSelector(+1);
        }
    }

public:
    InventoryContainer(std::shared_ptr<::Entity> entity_,
                       Components                children,
                       int                      *selector)
        : VerticalContainer(children, selector),
          inventory_{entity_->getComponent<::Inventory>()->inventory}
    {
        this->entity_ = entity_;
        it_           = inventory_.begin();
        popup_        = PopupType::NONE;
    }

    Element Render() override
    {
        Elements elements;
        for (auto &it : children_)
        {
            elements.push_back(it->Render());
        }
        if (elements.empty())
        {
            return text("Nothing seems to be there") | reflect(box_);
        }
        if (bool(popup_ & PopupType::DESCRIPTION))
        {
            return dbox(
                vbox(std::move(elements)) | reflect(box_),
                window(text((*it_)->getComponent<Name>()->name) |
                           color(getColor(
                               (*it_)->getComponent<ItemComponent>()->rarity)) |
                           center,
                       paragraphAlignJustify(
                           (*it_)->getComponent<Description>()->description)) |
                    clear_under | center | flex);
        }

        if (bool(popup_ & PopupType::STATS))
        {
            Elements stats;
            auto     item_component = (*it_)->getComponent<::ItemComponent>();

            auto effect_component   = (*it_)->getComponent<::EffectComponent>();
            auto armor_component    = (*it_)->getComponent<::ArmorComponent>();
            auto weapon_component   = (*it_)->getComponent<::WeaponComponent>();
            auto hunger_component   = (*it_)->getComponent<::HungerComponent>();

            if (effect_component != nullptr)
            {
                Elements effects;
                auto     effect = effect_component->effect;
                if (effect & Effect::HEAL)
                {
                    std::string effect_string =
                        " Heal " +
                        std::to_string(effect_component->effect_strength);
                    effect = effect ^ Effect::HEAL;
                    effects.push_back(text(effect_string) |
                                      color(Color::RedLight));
                }
                if (effect & Effect::IRONSKIN)
                {
                    std::string effect_string =
                        " Ironskin " +
                        std::to_string(effect_component->effect_strength);
                    effect = effect ^ Effect::IRONSKIN;
                    effects.push_back(text(effect_string) |
                                      color(Color::LightYellow3));
                }
                if (effect & Effect::BLEED)
                {
                    std::string effect_string =
                        " Bleed " +
                        std::to_string(effect_component->effect_strength);
                    effect = effect ^ Effect::BLEED;
                    effects.push_back(text(effect_string) |
                                      color(Color::DarkRed));
                }
                if (effect & Effect::BLIND)
                {
                    std::string effect_string =
                        " Blind " +
                        std::to_string(effect_component->effect_strength);
                    effect = effect ^ Effect::BLIND;
                    effects.push_back(text(effect_string) |
                                      color(Color::GrayLight));
                }
                if (effect & Effect::POISON)
                {
                    std::string effect_string =
                        " Poison " +
                        std::to_string(effect_component->effect_strength);
                    effect = effect ^ Effect::POISON;
                    effects.push_back(text(effect_string) |
                                      color(Color::MediumPurple));
                }

                if (!effects.empty())
                {
                    if (effects.size() > 1)
                    {
                        stats.push_back(text("Effects: "));
                    }
                    else
                        stats.push_back(text("Effect: "));
                    stats.push_back(hbox(effects));
                }
            }

            if (armor_component != nullptr)
            {
                stats.push_back(
                    hbox({text("Armor class: "),
                          text(std::to_string(armor_component->armor_class))}));
            }
            if (weapon_component != nullptr)
            {
                stats.push_back(
                    hbox({text("Weapon damage: "),
                          text(std::to_string(weapon_component->damage))}));
            }
            if (hunger_component != nullptr)
            {
                stats.push_back(
                    hbox({text("Food value: "),
                          text(std::to_string(hunger_component->hunger))}));
            }

            return dbox(
                vbox(std::move(elements)) | reflect(box_),
                window(text((*it_)->getComponent<Name>()->name) |
                           color(getColor(
                               (*it_)->getComponent<ItemComponent>()->rarity)) |
                           center,
                       vbox(stats)) |
                    clear_under | center | flex);
        }

        return vbox(std::move(elements)) | reflect(box_);
    }

    bool EventHandler(Event event) override
    {
        const int old_selected = *selector_;
        if (event == Event::ArrowUp || event == Event::Character('k'))
        {
            MoveSelector(-1);
        }
        if (event == Event::ArrowDown || event == Event::Character('j'))
        {
            MoveSelector(+1);
        }
        if (event == Event::PageUp)
        {
            for (int i = 0; i < box_.y_max - box_.y_min; ++i)
            {
                MoveSelector(-1);
            }
        }
        if (event == Event::PageDown)
        {
            for (int i = 0; i < box_.y_max - box_.y_min; ++i)
            {
                MoveSelector(1);
            }
        }
        if (event == Event::Home)
        {
            for (size_t i = 0; i < children_.size(); ++i)
            {
                MoveSelector(-1);
            }
        }
        if (event == Event::End)
        {
            for (size_t i = 0; i < children_.size(); ++i)
            {
                MoveSelector(1);
            }
        }
        if (event == Event::Tab)
        {
            MoveSelectorWrap(+1);
        }
        if (event == Event::TabReverse)
        {
            MoveSelectorWrap(-1);
        }

        // ===========================
        // overriden inputs start here
        // ===========================
        if (event == Event::Character('r'))
        {
            /*DROP*/
            auto item_iterator = it_;

            /*I will use this in conjunction with position system later*/
            InventorySystem::dropFromInventory(entity_, item_iterator);
            containerCorrection();
        }
        if (event == Event::Character('d'))
        {
            /*DESCRIPTION*/
            popup_ ^= PopupType::DESCRIPTION;
            // if (popup_ == PopupType::DESCRIPTION)
            // {
            //     popup_ = PopupType::NONE;
            // }
            // else
            //     popup_ = PopupType::DESCRIPTION;
        }
        if (event == Event::Character('s'))
        {
            /*STATS*/
            popup_ ^= PopupType::STATS;
        }
        if (event == Event::Return)
        {
            /*EQUIP*/
            auto size_before = inventory_.size();
            InventorySystem::useItem(entity_, it_);
            if (inventory_.size() != size_before)
            {
                containerCorrection();
            }
        }

        *selector_ =
            std::max(0, std::min(int(children_.size()) - 1, *selector_));
        return old_selected != *selector_;
    }
};

Component
Inventory(std::shared_ptr<::Entity> entity, Components children, int *selector)
{
    return std::make_shared<InventoryContainer>(
        entity, std::move(children), selector);
}
} // namespace ftxui::Container

MenuEntryOption inv_item_look(ItemComponent &item_component)
{

    Color col = getColor(item_component.rarity);

    ftxui::MenuEntryOption option;
    option.animated_colors.foreground.enabled = true;
    option.animated_colors.background.enabled = true;
    // option.animated_colors.foreground.active   = col;
    // option.animated_colors.foreground.inactive = col;
    // option.animated_colors.background.active   =
    // ftxui::Color::DarkSlateGray1; option.animated_colors.background.inactive
    // = ftxui::Color::Black;

    option.transform = [&item_component, col](EntryState state)
    {
        Element elem = text(state.label) | color(col);
        if (item_component.equipped)
        {
            elem = hbox({elem, text(" E") | color(Color::White)});
        }

        if (state.focused)
            elem = hbox({text("→ ") | color(Color::White), elem});

        return elem;
    };
    return option;
}

int main()
{
    std::shared_ptr<Entity> player(new Entity(PLAYER,
                                              {
                                                  new Inventory(),
                                                  new ArmorSlot(),
                                                  new WeaponSlot(),
                                                  new AmuletSlot(),
                                                  new BuffComponent(),
                                                  new Health(20, 5),
                                              }));

    std::shared_ptr<Entity> sword(new Entity(
        ITEM,
        {
            new Name("Sword of foo"),
            new Description("This sword does foo"),
            new WeaponComponent(10),
            new ItemComponent(
                ItemType::WEAPON, /*stack*/ 1, /*max_stack*/ 1, EPIC),

        }));

    std::shared_ptr<Entity> sword2(new Entity(
        ITEM,
        {
            new Name("Sword of foo"),
            new Description("This sword does foo"),
            new WeaponComponent(10),
            new ItemComponent(
                ItemType::WEAPON, /*stack*/ 1, /*max_stack*/ 1, COMMON),

        }));

    std::shared_ptr<Entity> armor(new Entity(
        ITEM,
        {
            new Name("Armor of bar"),
            new Description("This armor used to belong to bar"),
            new ArmorComponent(40),
            new ItemComponent(
                ItemType::ARMOR, /*stack*/ 1, /*max_stack*/ 1, LEGENDARY),
        }));

    std::shared_ptr<Entity> heal_potion(new Entity(
        ITEM,
        {
            new Name("Potion of healing"),
            new Description("A ruby-colored concoction, supposedly able to "
                            "heal injuries of those who drink it. It doesn't "
                            "smell very appetizing, though "),
            new EffectComponent(HEAL | IRONSKIN, 2, 0),
            new ItemComponent(ItemType::POTION, 1, 16, UNCOMMON),
        }));

    InventorySystem invsys;
    invsys.addToInventory(
        player->getComponent<Inventory>()->inventory,
        {sword, sword2, armor, heal_potion, heal_potion, heal_potion, heal_potion});

    std::cout << player->getComponent<Health>()->current_health_points;
    // invsys.useItem(player, 1);
    // invsys.useItem(player, 1);
    // invsys.dropFromInventory(player, 1);

    for (auto item : player->getComponent<Inventory>()->inventory)
    {
        auto item_component   = item->getComponent<ItemComponent>();
        auto item_name        = item->getComponent<Name>();
        auto item_description = item->getComponent<Description>();

        std::cout << item_name->name << std::endl
                  << item_description->description << std::endl
                  << item_component->stack << '/' << item_component->max_stack
                  << std::endl
                  << "Rarity: " << static_cast<int>(item_component->rarity)
                  << std::endl
                  << "Equipped: " << std::boolalpha << item_component->equipped
                  << std::endl
                  << std::endl;
    }
    HealthSystem::updateHealth(player, 4, ADD | CURRENT);
    std::cout << player->getComponent<Health>()->current_health_points;
    std::cout << "I didn't segfault!";

    using namespace ftxui;
    int menu_index   = 0;

    // auto menu = Menu(&entries, &menu_index);

    auto get_entries = [&](Components &entries)
    {
        entries.clear();
        for (const auto &item : player->getComponent<Inventory>()->inventory)
        {
            auto name      = item->getComponent<Name>()->name;
            auto item_comp = item->getComponent<ItemComponent>();
            entries.push_back(MenuEntry(name, inv_item_look(*item_comp)));
        }
        return entries;
    };

    Components entries;

    Components etr;
    for (const auto &item : player->getComponent<Inventory>()->inventory)
    {
        auto name      = item->getComponent<Name>()->name;
        auto item_comp = item->getComponent<ItemComponent>();
        etr.push_back(MenuEntry(name));
    }

    auto menu = Container::Inventory(player, get_entries(entries), &menu_index);

    auto screen = ScreenInteractive::Fullscreen();
    auto loop   = Loop(&screen, menu);

    // InventorySystem::useItem(player, 3);

    while (loop.HasQuitted() == false)
    {

        // menu = Container::Vertical(entries, &menu_index);
        // get_entries(entries);
        loop.RunOnce();
    }
}