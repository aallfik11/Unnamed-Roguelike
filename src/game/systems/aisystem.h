#ifndef AI_SYSTEM_H
#define AI_SYSTEM_H
#include "../action.h"
#include "../ai_enum.h"
#include "../components/aicomponent.h"
#include "../components/coordinates.h"
#include "../components/health.h"
#include "../components/lineofsightcomponent.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../globals.h"
#include "../system.h"
#include "navmapmanager.h"
#include <any>
#include <cstdint>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <unordered_set>

class AISystem : public System, public EntityHolder
{

    /* In order to work correctly, an entity in the AI System is required
     * to have the following components:
     * AIComponent
     * NavMapComponent
     * LOSComponent
     * Health
     * Coordinates
     * WeaponComponent
     * StrengthComponent
     */

    using AISet   = std::unordered_set<Entity *>;
    using GameMap = std::vector<std::vector<Tile>>;

    AISet               ais_;
    GameMap             map_;
    NavMapManager      &navigation_manager_;
    Entity             *player_;
    std::list<Entity *> addition_messages_;
    std::list<Entity *> removal_messages_;

public: // temporary
    double getRunThreshold(AIType &ai_type)
    {
        switch (ai_type)
        {
        case AIType::AI_MONSTER_BERSERK:
            return 0.0l;
            break;

        case AIType::AI_MONSTER_AGGRESSIVE:
            return 0.2l;
            break;

        case AIType::AI_MONSTER_COWARDLY:
            return 0.8l;
            break;

        default:
            return 0.5l;
        }
    }

    // todo: make the functions take a boolean to ignore checks (to make calling
    // them from other functions faster);

    void approachTarget(Entity *const caller, Entity *const target)
    {
        /*if player within range, attack
          if lost LOS of player, go to the last known coords
          if no player in LOS around last known coords, wander around
        */
        auto caller_coordinates = caller->getComponent<Coordinates>();
        auto target_coordinates = target->getComponent<Coordinates>();
        auto caller_brain       = caller->getComponent<AIComponent>();
        auto caller_LOS         = caller->getComponent<LOSComponent>();

        // if player within range, attack
        if (caller_LOS->has_LOS_to_player == true)
        {
            auto abs_distance_x =
                std::abs(caller_coordinates->x - target_coordinates->x);
            auto abs_distance_y =
                std::abs(caller_coordinates->y - target_coordinates->y);
            // ugly, I hope to come up with something better later

            if (abs_distance_x <= 1 && abs_distance_y <= 1)
            {
                caller_brain->ai_state = AIState::ATTACK;
                return attack(caller, target);
            }
            navigation_manager_.switchToPlayerNavMap(
                caller->getComponent<NavMapComponent>()->nav_map);
            auto caller_brain           = caller->getComponent<AIComponent>();
            caller_brain->last_target_x = target_coordinates->x;
            caller_brain->last_target_y = target_coordinates->y;
        }
        auto [x, y] = navigation_manager_.nextBestCoordinates(
            caller, NavMapManager::Destination::TOWARDS);

        if (caller_coordinates->x == caller_brain->last_target_x &&
            caller_coordinates->y == caller_brain->last_target_y)
        {
            auto caller_navmap     = caller->getComponent<NavMapComponent>();
            caller_brain->ai_state = AIState::WANDER_AROUND;
            navigation_manager_.assignRandomTarget(caller_navmap->nav_map);
            return wanderAround(caller, target);
        }
        // positon_system_.updatePosition(caller, x, y);
        auto message = {std::make_any<SystemAction::POSITION>(
                            SystemAction::POSITION::UPDATE),
                        std::make_any<Entity *>(caller),
                        std::make_any<uint16_t>(x),
                        std::make_any<uint16_t>(y)};
        System::sendSystemMessage(SystemType::POSITION, message);
    }

    void runAway(Entity *const caller, Entity *const target)
    {
        /*if there's a line of sight to the player, run away, else rest
         * if no way of running exists (backed into a corner),
         * if not cowardly enter berserk mode
         */
        if (caller->getComponent<LOSComponent>()->has_LOS_to_player == false)
        {
            caller->getComponent<AIComponent>()->ai_state = AIState::REST;
            return rest(caller, target);
        }

        auto [x, y] = navigation_manager_.nextBestCoordinates(
            caller, NavMapManager::Destination::AWAY_FROM);
        if ((map_[x][y].type & (TileType::HAS_CREATURE |
                                TileType::TRAVERSIBLE)) != TileType::NONE)
        {
            auto caller_brain = caller->getComponent<AIComponent>();
            if (caller_brain->ai_type != AIType::AI_MONSTER_COWARDLY)
            {
                caller_brain->ai_type  = AIType::AI_MONSTER_BERSERK;
                caller_brain->ai_state = AIState::ATTACK;
                return attack(caller, target);
            }
        }
    }

    void rest(Entity *const caller, Entity *const target)
    {
        /*if hp high enough and there's a LOS to the player, approach
         * if hp low and LOS to player then run away
         * if hp high enough and no LOS to the player, wander around
         */
        auto   caller_brain  = caller->getComponent<AIComponent>();
        auto   LOS           = caller->getComponent<LOSComponent>();
        auto   caller_health = caller->getComponent<Health>();
        double run_threshold = getRunThreshold(caller_brain->ai_type);

        double current_hp_percentage =
            (static_cast<double>(caller_health->current_health_points) /
             static_cast<double>(caller_health->max_health_points));

        if (LOS->has_LOS_to_player == true)
        {
            auto caller_navmap      = caller->getComponent<NavMapComponent>();
            auto target_coordinates = target->getComponent<Coordinates>();
            navigation_manager_.switchToPlayerNavMap(caller_navmap->nav_map);

            caller_brain->last_target_x = target_coordinates->x;
            caller_brain->last_target_y = target_coordinates->y;
            if (current_hp_percentage > run_threshold)
            {
                caller_brain->ai_state = AIState::RUN_AWAY;
                return runAway(caller, target);
            }

            caller_brain->ai_state = AIState::APPROACH_TARGET;

            return approachTarget(caller, target);
        }

        if (current_hp_percentage >= 0.99)
        {
            navigation_manager_.assignRandomTarget(
                caller->getComponent<NavMapComponent>()->nav_map);
            caller_brain->ai_state = AIState::WANDER_AROUND;
            return wanderAround(caller, target);
        }

        // caller_health->current_health_points += 1;
        auto message = {
            std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::UPDATE),
            std::make_any<Entity *>(caller),
            std::make_any<uint16_t>(1),
            std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::HEAL |
                                                SystemAction::HEALTH::CURRENT)};
        System::sendSystemMessage(SystemType::HEALTH, message);
        // might want to add a regen_amount component (or a field to
        // Health component) later
    }

    void attack(Entity *const caller, Entity *const target)
    {
        /*if hp goes low, run away
         *if player runs away, chase,
         */
        auto caller_brain       = caller->getComponent<AIComponent>();
        auto caller_health      = caller->getComponent<Health>();
        auto caller_position    = caller->getComponent<Coordinates>();
        auto target_coordinates = target->getComponent<Coordinates>();

        auto abs_dist_x = std::abs(caller_position->x - target_coordinates->x);
        auto abs_dist_y = std::abs(caller_position->y - target_coordinates->y);

        double run_threshold = getRunThreshold(caller_brain->ai_type);
        double current_hp_percentage =
            (static_cast<double>(caller_health->current_health_points) /
             static_cast<double>(caller_health->max_health_points));

        if (current_hp_percentage <= run_threshold)
        {
            caller_brain->ai_state = AIState::RUN_AWAY;
            return runAway(caller, target);
        }

        if (abs_dist_x > 1 || abs_dist_y > 1)
        {
            caller_brain->ai_state = AIState::APPROACH_TARGET;

            navigation_manager_.switchToPlayerNavMap(
                caller->getComponent<NavMapComponent>()->nav_map);
            caller_brain->last_target_x = target_coordinates->x;
            caller_brain->last_target_y = target_coordinates->y;
            return approachTarget(caller, target);
        }

        // AttackSystem::attack(caller, target);
        auto message = {std::make_any<Entity *>(caller),
                        std::make_any<Entity *>(target)};
        System::sendSystemMessage(SystemType::ATTACK, message);
    }

    void wanderAround(Entity *const caller, Entity *const target)
    {
        /*if player enters LOS, approach/run away
         *(depending on the circumstances)
         */
        auto LOS                = caller->getComponent<LOSComponent>();
        auto caller_brain       = caller->getComponent<AIComponent>();
        auto target_coordinates = target->getComponent<Coordinates>();

        if (LOS->has_LOS_to_player == true)
        {
            auto caller_health   = caller->getComponent<Health>();
            auto caller_navmap   = caller->getComponent<NavMapComponent>();

            double run_threshold = getRunThreshold(caller_brain->ai_type);
            double current_hp_percentage =
                (static_cast<double>(caller_health->current_health_points) /
                 static_cast<double>(caller_health->max_health_points));

            navigation_manager_.switchToPlayerNavMap(caller_navmap->nav_map);

            caller_brain->last_target_x = target_coordinates->x;
            caller_brain->last_target_y = target_coordinates->y;

            if (current_hp_percentage > run_threshold)
            {
                caller_brain->ai_state = AIState::APPROACH_TARGET;
                return approachTarget(caller, target);
            }

            caller_brain->ai_state = AIState::RUN_AWAY;
            return runAway(caller, target);
        }

        // entities will have a roaming navigation map assigned by default
        auto caller_coords = caller->getComponent<Coordinates>();
        auto [x, y]        = navigation_manager_.nextBestCoordinates(
            caller, NavMapManager::Destination::TOWARDS);
        if (caller_coords->x == x && caller_coords->y == y)
        {
            auto caller_navmap = caller->getComponent<NavMapComponent>();
            navigation_manager_.assignRandomTarget(caller_navmap->nav_map);
            auto [x2, y2] = navigation_manager_.nextBestCoordinates(
                caller, NavMapManager::Destination::TOWARDS);
            x = x2;
            y = y2;
        }
        // positon_system_.updatePosition(caller, x, y);

        auto message = {std::make_any<SystemAction::POSITION>(
                            SystemAction::POSITION::UPDATE),
                        std::make_any<Entity *>(caller),
                        std::make_any<uint16_t>(x),
                        std::make_any<uint16_t>(y)};
        System::sendSystemMessage(SystemType::POSITION, message);
    }

    // void special(Entity * const caller, Entity * const target)
    // {
    //     /*unused for now
    //      */
    // }

    // void interactWithObject(Entity * const caller, Entity * const target)
    // {
    //     /*unused for now
    //      */
    // }

public:
    AISystem(GameMap &map, NavMapManager &nav_manager, Entity *const player)
        : map_{map}, navigation_manager_{nav_manager}, player_{player}
    {
        // states_[APPROACH_TARGET] = std::function<Action(EntityPtr&,
        // EntityPtr&)>(approachTarget); states_[RUN_AWAY]        =
        // std::function<Action(EntityPtr&, EntityPtr&)>(runAway);
        // states_[WANDER_AROUND]   = std::function<Action(EntityPtr&,
        // EntityPtr&)>(wanderAround); states_[REST]            =
        // std::function<Action(EntityPtr&, EntityPtr&)>(rest);
        // states_[ATTACK] = std::function<Action(EntityPtr&,
        // EntityPtr&)>(attack); This isn't necessary;
        // /*APPROACH TARGET*/
        // states_[APPROACH_TARGET].emplace(ATTACK, attack); // if player
        // close enough
        // states_[APPROACH_TARGET].emplace(INTERACT_WITH_OBJECT,
        // interactWithObject);
        // states_[APPROACH_TARGET].emplace(WANDER_AROUND, wanderAround);
        // /*APPROACH TARGET*/

        // /*RUN_AWAY*/
        // states_[RUN_AWAY].emplace(REST, rest); // if far enough from the
        // player
        // /*RUN_AWAY*/

        // /*REST*/
        // states_[REST].emplace(APPROACH_TARGET, approachTarget); // if
        // healthy enough or target has high priority
        // states_[REST].emplace(RUN_AWAY, runAway);               // if not
        // healthy enough to fight and/or afraid/cowardly
        // states_[REST].emplace(WANDER_AROUND, wanderAround);
        // // if healthy and nothing else to do
        // /*REST*/

        // /*ATTACK*/
        // states_[ATTACK].emplace(RUN_AWAY, runAway);               // if
        // not healthy enough states_[ATTACK].emplace(APPROACH_TARGET,
        // approachTarget); // if target runs away and healthy enough
        // states_[ATTACK].emplace(SPECIAL, special);                // if
        // special conditions apply
        // /*ATTACK*/

        // /*INTERACT*/
        // states_[INTERACT_WITH_OBJECT].emplace(WANDER_AROUND,
        // wanderAround);
        // // if done interacting and nothing else to do
        // states_[INTERACT_WITH_OBJECT].emplace(APPROACH_TARGET,
        // approachTarget); // if a valid target in range
        // states_[INTERACT_WITH_OBJECT].emplace(ATTACK, attack); // if a
        // valid target directly near
        // states_[INTERACT_WITH_OBJECT].emplace(RUN_AWAY, runAway); // if
        // not healthy enough to fight and/or afraid/cowardly
        // /*INTERACT*/

        // /*WANDER AROUND*/
        // states_[WANDER_AROUND].emplace(INTERACT_WITH_OBJECT,
        // interactWithObject); // if stumbled upon something interactable
        // (might not use that)
        // states_[WANDER_AROUND].emplace(APPROACH_TARGET, approachTarget);
        // // if a valid target in range
        // states_[WANDER_AROUND].emplace(RUN_AWAY, runAway); // if not
        // healthy enough to fight and/or afraid/cowardly
        // /*WANDER AROUND*/

        // /*SPECIAL*/
        // states_[SPECIAL].emplace(APPROACH_TARGET, approachTarget); // if
        // target not withing special range
        // states_[SPECIAL].emplace(RUN_AWAY, runAway);               //
        // depending on the special action states_[SPECIAL].emplace(ATTACK,
        // attack);                  // depending on the special action
        // states_[SPECIAL].emplace(REST, rest);
        // // depending on the special action
        // /*SPECIAL*/
    }

    inline void addEntity(Entity *const entity) { ais_.emplace(entity); }

    inline void deleteEntity(Entity *const entity) { ais_.erase(entity); }

    void runAI(Entity *const caller, Entity *const target)
    {
        if (!ais_.contains(caller))
            return;

        switch (caller->getComponent<AIComponent>()->ai_state)
        {
        case AIState::APPROACH_TARGET:

            approachTarget(caller, target);
            break;

        case AIState::RUN_AWAY:

            runAway(caller, target);
            break;

        case AIState::REST:

            rest(caller, target);
            break;

        case AIState::WANDER_AROUND:

            wanderAround(caller, target);
            break;

        case AIState::ATTACK:

            attack(caller, target);
            break;
        }
    }
    void updateData() override
    {
        for (auto &entity : removal_messages_)
        {
            deleteEntity(entity);
        }

        for (auto &entity : addition_messages_)
        {
            addEntity(entity);
        }

        for (auto &ai : ais_)
        {
            runAI(ai, player_);
        }
    }
    void readSystemMessages() override
    {
        for (auto &message : (*System::system_messages_)[SystemType::AI])
        {
            auto message_it   = message.begin();
            auto message_type = std::any_cast<SystemAction::AI>(*message_it);
            ++message_it;

            switch (message_type)
            {
            case SystemAction::AI::ADD:
            {
                addition_messages_.emplace_back(
                    std::any_cast<Entity *>(*message_it));
                break;
            }
            case SystemAction::AI::REMOVE:
            {
                removal_messages_.emplace_back(
                    std::any_cast<Entity *>(*message_it));
                break;
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::AI].clear();
        addition_messages_.clear();
        removal_messages_.clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::AI << ' ' << ais_.size() << ' ';
        for (auto &entity : ais_)
        {
            os << entity->getId() << ' ';
        }
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        std::size_t ais_size{};
        is >> ais_size;
        if (ais_size == 0)
        {
            return is;
        }
        std::shared_ptr<std::list<uint32_t>> entity_requests(
            new std::list<uint32_t>);
        uint32_t temp{};
        for (std::size_t i = 0; i < ais_size; i++)
        {
            is >> temp;
            entity_requests->push_back(temp);
        }
        auto message = {
            std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
            std::make_any<EntityHolder *>(this),
            std::make_any<std::shared_ptr<std::list<uint32_t>>>(
                entity_requests)};
        System::sendSystemMessage(SystemType::ENTITY, message);
        return is;
    }

    void loadEntities(std::shared_ptr<std::list<Entity *>> &entities) override
    {
        for (auto &entity : *entities)
        {
            addEntity(entity);
        }
    }
};

#endif /*AI_SYSTEM_H*/
