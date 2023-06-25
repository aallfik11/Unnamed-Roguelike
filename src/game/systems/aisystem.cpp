#include "aisystem.h"
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
#include <cstddef>
#include <cstdint>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <unordered_set>

using AISet   = std::unordered_set<observer_ptr<Entity>>;
using GameMap = std::vector<std::vector<Tile>>;

double AISystem::getRunThreshold(AIType &ai_type)
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

void AISystem::approachTarget(const observer_ptr<Entity> caller,
                              const observer_ptr<Entity> target)
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
        navigation_manager_->switchToPlayerNavMap(
            caller->getComponent<NavMapComponent>()->nav_map);
        auto caller_brain           = caller->getComponent<AIComponent>();
        caller_brain->last_target_x = target_coordinates->x;
        caller_brain->last_target_y = target_coordinates->y;
    }
    auto [x, y] = navigation_manager_->nextBestCoordinates(
        caller, NavMapManager::Destination::TOWARDS);

    if (caller_coordinates->x == caller_brain->last_target_x &&
        caller_coordinates->y == caller_brain->last_target_y)
    {
        auto caller_navmap     = caller->getComponent<NavMapComponent>();
        caller_brain->ai_state = AIState::WANDER_AROUND;
        navigation_manager_->assignRandomTarget(caller_navmap->nav_map);
        return wanderAround(caller, target);
    }
    // positon_system_.updatePosition(caller, x, y);
    auto message = {
        std::make_any<SystemAction::POSITION>(SystemAction::POSITION::UPDATE),
        std::make_any<observer_ptr<Entity>>(caller),
        std::make_any<uint16_t>(x),
        std::make_any<uint16_t>(y)};
    System::sendSystemMessage(SystemType::POSITION, message);
}

void AISystem::runAway(const observer_ptr<Entity> caller,
                       const observer_ptr<Entity> target)
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

    navigation_manager_->switchToPlayerNavMap(
        caller->getComponent<NavMapComponent>()->nav_map);

    auto [x, y] = navigation_manager_->nextBestCoordinates(
        caller, NavMapManager::Destination::AWAY_FROM);
    if (((*map_)[x][y].type & (TileType::HAS_CREATURE | TileType::WALL)) !=
        TileType::NONE)
    {
        auto caller_brain = caller->getComponent<AIComponent>();
        if (caller_brain->ai_type != AIType::AI_MONSTER_COWARDLY)
        {
            caller_brain->ai_type  = AIType::AI_MONSTER_BERSERK;
            caller_brain->ai_state = AIState::ATTACK;
            return attack(caller, target);
        }
    }
    else
    {
        auto message = {std::make_any<SystemAction::POSITION>(
                            SystemAction::POSITION::UPDATE),
                        std::make_any<observer_ptr<Entity>>(caller),
                        std::make_any<uint16_t>(x),
                        std::make_any<uint16_t>(y)};
        System::sendSystemMessage(SystemType::POSITION, message);
    }
}

void AISystem::rest(const observer_ptr<Entity> caller,
                    const observer_ptr<Entity> target)
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
        navigation_manager_->switchToPlayerNavMap(caller_navmap->nav_map);

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
        navigation_manager_->assignRandomTarget(
            caller->getComponent<NavMapComponent>()->nav_map);
        caller_brain->ai_state = AIState::WANDER_AROUND;
        return wanderAround(caller, target);
    }

    // caller_health->current_health_points += 1;
    auto message = {
        std::make_any<observer_ptr<Entity>>(caller),
        std::make_any<uint16_t>(1),
        std::make_any<SystemAction::HEALTH>(SystemAction::HEALTH::HEAL |
                                            SystemAction::HEALTH::CURRENT)};
    System::sendSystemMessage(SystemType::HEALTH, message);
    // might want to add a regen_amount component (or a field to
    // Health component) later
}

void AISystem::attack(const observer_ptr<Entity> caller,
                      const observer_ptr<Entity> target)
{
    /*if hp goes low, run away
     *if player runs away, chase,
     */
    auto caller_brain       = caller->getComponent<AIComponent>();
    auto caller_health      = caller->getComponent<Health>();
    auto caller_position    = caller->getComponent<Coordinates>();
    auto target_coordinates = target->getComponent<Coordinates>();

    auto abs_dist_x      = std::abs(caller_position->x - target_coordinates->x);
    auto abs_dist_y      = std::abs(caller_position->y - target_coordinates->y);

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

        navigation_manager_->switchToPlayerNavMap(
            caller->getComponent<NavMapComponent>()->nav_map);
        caller_brain->last_target_x = target_coordinates->x;
        caller_brain->last_target_y = target_coordinates->y;
        return approachTarget(caller, target);
    }

    // AttackSystem::attack(caller, target);
    auto message = {std::make_any<observer_ptr<Entity>>(caller),
                    std::make_any<observer_ptr<Entity>>(target)};
    System::sendSystemMessage(SystemType::ATTACK, message);
}

void AISystem::wanderAround(const observer_ptr<Entity> caller,
                            const observer_ptr<Entity> target)
{
    /*if player enters LOS, approach/run away
     *(depending on the circumstances)
     */
    auto line_of_sight      = caller->getComponent<LOSComponent>();
    auto caller_brain       = caller->getComponent<AIComponent>();
    auto target_coordinates = target->getComponent<Coordinates>();

    if (line_of_sight->has_LOS_to_player == true)
    {
        auto caller_health   = caller->getComponent<Health>();
        auto caller_navmap   = caller->getComponent<NavMapComponent>();

        double run_threshold = getRunThreshold(caller_brain->ai_type);
        double current_hp_percentage =
            (static_cast<double>(caller_health->current_health_points) /
             static_cast<double>(caller_health->max_health_points));

        navigation_manager_->switchToPlayerNavMap(caller_navmap->nav_map);

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
    auto [x, y]        = navigation_manager_->nextBestCoordinates(
        caller, NavMapManager::Destination::TOWARDS);
    if (caller_coords->x == x && caller_coords->y == y)
    {
        auto caller_navmap = caller->getComponent<NavMapComponent>();
        navigation_manager_->assignRandomTarget(caller_navmap->nav_map);
        auto [x2, y2] = navigation_manager_->nextBestCoordinates(
            caller, NavMapManager::Destination::TOWARDS);
        x = x2;
        y = y2;
    }

    auto message = {
        std::make_any<SystemAction::POSITION>(SystemAction::POSITION::UPDATE),
        std::make_any<observer_ptr<Entity>>(caller),
        std::make_any<uint16_t>(x),
        std::make_any<uint16_t>(y)};
    System::sendSystemMessage(SystemType::POSITION, message);
}

AISystem::AISystem()
{
    map_                = nullptr;
    navigation_manager_ = nullptr;
    player_             = nullptr;
}
AISystem::AISystem(const observer_ptr<const GameMap> map,
                   observer_ptr<NavMapManager>       nav_manager,
                   observer_ptr<Entity> const        player)
{
    map_                = map;
    navigation_manager_ = nav_manager;
    player_             = player;
}

void AISystem::assignPlayer(const observer_ptr<Entity> player)
{
    this->player_ = player;
}

void AISystem::assignNavMapManager(
    const observer_ptr<NavMapManager> nav_manager)
{
    this->navigation_manager_ = nav_manager;
}

void AISystem::assignMap(const observer_ptr<const GameMap> map)
{
    this->map_ = map;
}

inline void AISystem::addEntity(const observer_ptr<Entity> entity)
{
    navigation_manager_->assignRandomTarget(
        entity->getComponent<NavMapComponent>()->nav_map);
    ais_.emplace(entity);
}

inline void AISystem::deleteEntity(const observer_ptr<Entity> entity)
{
    ais_.erase(entity);
}

void AISystem::runAI(const observer_ptr<Entity> caller,
                     const observer_ptr<Entity> target)
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
void AISystem::updateData() noexcept(false)
{
    if (map_ == nullptr)
        throw std::runtime_error("AI System: ERROR -> Map unassigned");
    if (navigation_manager_ == nullptr)
        throw std::runtime_error(
            "AI System: ERROR -> NavMapManager unassigned");
    if (player_ == nullptr)
        throw std::runtime_error("AI System: ERROR -> Player unassigned");

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
void AISystem::readSystemMessages()
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
                std::any_cast<observer_ptr<Entity>>(*message_it));
            break;
        }
        case SystemAction::AI::REMOVE:
        {
            removal_messages_.emplace_back(
                std::any_cast<observer_ptr<Entity>>(*message_it));
            break;
        }
        }
    }
}
void AISystem::clearSystemMessages()
{
    (*system_messages_)[SystemType::AI].clear();
    addition_messages_.clear();
    removal_messages_.clear();
}

void AISystem::resetSystem()
{
    clearSystemMessages();
    ais_.clear();
    map_                = nullptr;
    navigation_manager_ = nullptr;
    player_             = nullptr;
}

std::ostream &AISystem::serialize(std::ostream &os) const
{
    os << SystemType::AI << ' ' << ais_.size() << ' ';
    for (auto &entity : ais_)
    {
        os << entity->getId() << ' ';
    }
    return os;
}
std::istream &AISystem::deserialize(std::istream &is)
{
    std::size_t ais_size{};
    is >> ais_size;
    if (ais_size == 0)
    {
        return is;
    }
    ais_.clear();
    ais_.reserve(ais_size);
    std::list<uint32_t> entity_requests;
    uint32_t            temp{};
    for (std::size_t i = 0; i < ais_size; i++)
    {
        is >> temp;
        entity_requests.push_back(temp);
    }
    auto message = {
        std::make_any<SystemAction::ENTITY>(SystemAction::ENTITY::REQUEST),
        std::make_any<observer_ptr<EntityHolder>>(this),
        std::make_any<std::list<uint32_t>>(entity_requests)};
    System::sendSystemMessage(SystemType::ENTITY, message);
    return is;
}

void AISystem::loadEntities(std::list<observer_ptr<Entity>> entities)
{
    for (auto &entity : entities)
    {
        addEntity(entity);
    }
}
