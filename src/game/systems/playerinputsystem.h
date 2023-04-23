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
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <list>
#include <memory>

class PlayerControlSystem : public System
{
    // using EntityPtr = std::shared_ptr<Entity>;
    Entity                       *player_;
    ftxui::Component              inv_renderer_;
    ftxui::Component              inv_input_handler_;
    int                           inv_index_;
    std::list<Entity *>::iterator inv_iterator_;

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
                auto message = {std::make_any<Entity *>(player_),
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
                /* in order to save, first have all entites stored in a file,
                 * when reading put them into a map by their entity id, and then
                 * wherever else they were stored, put their entity id instead
                 * of the entity again.
                 */
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

    inline ftxui::Color getItemColor(const Rarity &rarity) const
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
    getItemAppearance(const ItemComponent *const item_component) const
    {
        using namespace ftxui;
        Color col = getItemColor(item_component->rarity);

        MenuEntryOption option;
        option.animated_colors.foreground.enabled = true;
        option.animated_colors.background.enabled = true;

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
    PlayerControlSystem(Entity *const player, ftxui::Component &main_screen)
    {
        player_ = player;

        using namespace ftxui;

        inv_renderer_ = Renderer(
            main_screen,
            [&]
            {
                auto &player_inventory =
                    player_->getComponent<Inventory>()->inventory;

                inv_iterator_ = player_inventory.begin();
                inv_index_    = 0;

                Components entries;
                for (auto &item : player_inventory)
                {
                    auto &name           = item->getComponent<Name>()->name;
                    auto  item_component = item->getComponent<ItemComponent>();
                    entries.emplace_back(
                        MenuEntry(name, getItemAppearance(item_component)));
                }

                auto inv_container = Container::Vertical(entries, &inv_index_);

                return dbox(main_screen->Render(),
                            inv_container->Render() | center | flex_shrink |
                                clear_under);
            });

        auto iterateToItem = [&](int8_t n)
        {
            auto size = player_->getComponent<Inventory>()->inventory.size();

            if (inv_index_ + n < size && inv_index_ + n >= 0)
            {
                (n > 0) ? inv_iterator_++ : inv_iterator_--;
                inv_index_ += n;
            }
        };

        inv_input_handler_ = CatchEvent(inv_renderer_,
                                        [&](Event event)
                                        {
                                            if (event.is_mouse())
                                                return false;

                                            if (event == Event::ArrowUp)
                                            {
                                                iterateToItem(-1);
                                                return true;
                                            }

                                            if (event == Event::ArrowDown)
                                            {
                                                iterateToItem(1);
                                                return true;
                                            }

                                            if (event == Event::Character('d'))
                                            {
                                                iterateToItem(1);
                                                return true;
                                            }
                                            if (event == Event::Character('s'))
                                            {
                                                iterateToItem(1);
                                                return true;
                                            }

                                            if (event == Event::Character('r'))
                                            {
                                                // drop item
                                                return true;
                                            }
                                            return false;
                                        });
    }
    void updateData() override {}
    void readSystemMessages() override {}
    void clearSystemMessages() override {}
};

#endif /*PLAYERCONTROLSYSTEM_H*/
