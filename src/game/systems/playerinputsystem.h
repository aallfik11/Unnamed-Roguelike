#ifndef PLAYERCONTROLSYSTEM_H
#define PLAYERCONTROLSYSTEM_H

#include "../components/armorcomponent.h"
#include "../components/armorslot.h"
#include "../components/coordinates.h"
#include "../components/description.h"
#include "../components/health.h"
#include "../components/inventory.h"
#include "../components/itemcomponent.h"
#include "../components/name.h"
#include "../components/weaponcomponent.h"
#include "../components/weaponslot.h"
#include "../directions.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../itemtypes.h"
#include "../rarity.h"
#include "../system.h"
#include <cmath>
#include <cstdint>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <istream>
#include <list>
#include <memory>

class PlayerControlSystem : public System, public EntityHolder
{
    observer_ptr<Entity>                      player_;
    ftxui::Component                          inv_renderer_;
    ftxui::Component                          inv_input_handler_;
    int                                       inv_index_;
    std::list<observer_ptr<Entity>>::iterator inv_iterator_;
    std::list<observer_ptr<Entity>>           entities_up_;
    std::list<observer_ptr<Entity>>           entities_down_;
    std::list<observer_ptr<Entity>>           entities_left_;
    std::list<observer_ptr<Entity>>           entities_right_;
    Direction                                 next_movement_;
    observer_ptr<Entity>                      last_hit_enemy_;
    int                                       last_hit_entity_timer_;

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
                auto message = {std::make_any<observer_ptr<Entity>>(player_),
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
        case Rarity::COMMON:
            return ftxui::Color::GrayLight;
        case Rarity::UNCOMMON:
            return ftxui::Color::Green3;
        case Rarity::RARE:
            return ftxui::Color::Aquamarine1;
        case Rarity::EPIC:
            return ftxui::Color::Purple;
        case Rarity::LEGENDARY:
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
    PlayerControlSystem(observer_ptr<Entity> const player,
                        ftxui::Component          &main_screen)
    {
        player_                = player;
        next_movement_         = Direction::NONE;
        last_hit_entity_timer_ = 0;

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

    EntityType checkMovementDestination(
        const std::list<observer_ptr<Entity>> &entities_at_direction)
    {
        if (entities_at_direction.empty())
            return EntityType::NONE;

        for (auto &entity : entities_at_direction)
        {
            if ((entity->type & EntityType::CREATURE) != EntityType::NONE &&
                (entity->type & EntityType::KILLED) == EntityType::NONE)
            {
                last_hit_enemy_        = entity;
                last_hit_entity_timer_ = 5;
                return EntityType::CREATURE;
            }
        }
        return EntityType::ITEM;
    }

    void updateData() override
    {
        EntityType entity_type_at_destination = EntityType::NONE;
        std::list<observer_ptr<Entity>> entities_at_destination;
        auto     coordinates = player_->getComponent<Coordinates>();
        uint16_t next_x      = coordinates->x;
        uint16_t next_y      = coordinates->y;
        switch (next_movement_)
        {
        case Direction::UP:
        {
            entity_type_at_destination = checkMovementDestination(entities_up_);
            entities_at_destination    = entities_up_;
            --next_y;
            break;
        }
        case Direction::DOWN:
        {
            entity_type_at_destination =
                checkMovementDestination(entities_down_);
            entities_at_destination = entities_down_;
            ++next_y;
            break;
        }
        case Direction::LEFT:
        {
            entity_type_at_destination =
                checkMovementDestination(entities_left_);
            entities_at_destination = entities_left_;
            --next_x;
            break;
        }
        case Direction::RIGHT:
        {
            entity_type_at_destination =
                checkMovementDestination(entities_right_);
            entities_at_destination = entities_right_;
            ++next_x;
            break;
        }
        case Direction::NONE:
        {
            return;
        }
        }
        switch (entity_type_at_destination)
        {
        case EntityType::CREATURE:
        {
            System::sendSystemMessage(
                SystemType::ATTACK,
                {std::make_any<observer_ptr<Entity>>(player_),
                 std::make_any<observer_ptr<Entity>>(last_hit_enemy_)});
            next_x = coordinates->x;
            next_y = coordinates->y;
            break;
        }
        case EntityType::ITEM:
        {
            System::sendSystemMessage(
                SystemType::INVENTORY,
                {std::make_any<SystemAction::INVENTORY>(
                     SystemAction::INVENTORY::ADD),
                 std::make_any<std::list<observer_ptr<Entity>>>(
                     entities_at_destination)});
        }
        case EntityType::NONE:
        {
            System::sendSystemMessage(
                SystemType::POSITION,
                {std::make_any<SystemAction::POSITION>(
                     SystemAction::POSITION::UPDATE),
                 std::make_any<observer_ptr<Entity>>(player_),
                 std::make_any<uint16_t>(next_x),
                 std::make_any<uint16_t>(next_y)});
            break;
        }
        }
        System::sendSystemMessage(
            SystemType::POSITION,
            {std::make_any<SystemAction::POSITION>(
                 SystemAction::POSITION::RECEIVE_PLAYER_COORDINATES),
             std::make_any<uint16_t>(next_x),
             std::make_any<uint16_t>(next_y)});
    }
    void readSystemMessages() override
    {
        for (auto &message : (*System::system_messages_)[SystemType::PLAYER])
        {
            auto message_it = message.begin();
            auto message_type =
                std::any_cast<SystemAction::PLAYER>(*message_it);
            ++message_it;
            switch (message_type)
            {
            case SystemAction::PLAYER::ENTITIES_UP:
            {
                entities_up_ =
                    std::any_cast<std::list<observer_ptr<Entity>>>(*message_it);
                break;
            }
            case SystemAction::PLAYER::ENTITIES_DOWN:
            {
                entities_down_ =
                    std::any_cast<std::list<observer_ptr<Entity>>>(*message_it);
                break;
            }
            case SystemAction::PLAYER::ENTITIES_LEFT:
            {
                entities_left_ =
                    std::any_cast<std::list<observer_ptr<Entity>>>(*message_it);
                break;
            }
            case SystemAction::PLAYER::ENTITIES_RIGHT:
            {
                entities_right_ =
                    std::any_cast<std::list<observer_ptr<Entity>>>(*message_it);
                break;
            }
            case SystemAction::PLAYER::MOVE:
            {
                next_movement_ = std::any_cast<Direction>(*message_it);
                break;
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*System::system_messages_)[SystemType::PLAYER].clear();
        entities_up_.clear();
        entities_down_.clear();
        entities_left_.clear();
        entities_right_.clear();
        next_movement_ = Direction::NONE;
        if (last_hit_enemy_ != nullptr)
        {
            if (last_hit_enemy_->getComponent<Health>()->alive == false)
            {
                last_hit_enemy_ = nullptr;
            }
            else if (last_hit_entity_timer_ == 0)
            {
                last_hit_enemy_ = nullptr;
            }
            else
                --last_hit_entity_timer_;
        }
    }
    std::istream &deserialize(std::istream &is) override
    {
        sendSystemMessage(
            SystemType::ENTITY,
            {std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
             std::make_any<observer_ptr<EntityHolder>>(this),
             std::make_any<std::list<uint32_t>>(std::list<uint32_t>({1}))});
        return is;
    }
    void loadEntities(std::list<observer_ptr<Entity>> player) override
    {
        this->player_ = player.front();
    }
};

#endif /*PLAYERCONTROLSYSTEM_H*/
