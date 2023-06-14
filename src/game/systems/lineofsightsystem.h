#ifndef LOSSYSTEM_H
#define LOSSYSTEM_H
#include "../components/coordinates.h"
#include "../components/lineofsightcomponent.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../globals.h"
#include "../system.h"
#include "../tile.h"
#include <cmath>
#include <istream>
#include <list>
#include <memory>
#include <ostream>
#include <unordered_set>

class LOS_System : public System, public EntityHolder
{
    using GameMap = std::vector<std::vector<Tile>>;

    observer_ptr<Entity>                     player_;
    std::list<std::pair<uint16_t, uint16_t>> visited_list_;
    std::unordered_set<observer_ptr<Entity>> lines_of_sight_;
    std::list<observer_ptr<Entity>>          addition_messages_;
    std::list<observer_ptr<Entity>>          deletion_messages_;

    observer_ptr<GameMap> map_;

    bool lineOfSightAlg(uint16_t init_x,
                        uint16_t init_y,
                        uint16_t target_x,
                        uint16_t target_y,
                        uint16_t seeing_distance) const
    {
        uint16_t distance_x = std::abs(init_x - target_x);
        uint16_t distance_y = std::abs(init_y - target_y);
        int8_t   sign_x     = (init_x > target_x) ? -1 : 1;
        int8_t   sign_y     = (init_y > target_y) ? -1 : 1;
        uint16_t x          = init_x;
        uint16_t y          = init_y;

        if (sqrt(pow(distance_x, 2) + pow(distance_y, 2)) > seeing_distance)
        {
            return false;
        }

        if (distance_x > distance_y)
        {
            int32_t t = distance_y * 2 - distance_x;

            do
            {
                if (((*map_)[x][y].type & TileType::TRAVERSIBLE) ==
                    TileType::NONE)
                    return false;

                if (t >= 0)
                {
                    y += sign_y;
                    t -= distance_x * 2;
                }
                x += sign_x;
                t += distance_y * 2;
            } while (x != target_x || y != target_y);
        }
        else
        {
            int32_t t = distance_x * 2 - distance_y;

            do
            {
                if (((*map_)[x][y].type & TileType::TRAVERSIBLE) ==
                    TileType::NONE)
                    return false;

                if (t >= 0)
                {
                    x += sign_x;
                    t -= distance_y * 2;
                }
                y += sign_y;
                t += distance_x * 2;
            } while (x != target_x || y != target_y);
        }

        return true;
    }

public:
    LOS_System()
    {
        map_    = nullptr;
        player_ = nullptr;
    }
    LOS_System(observer_ptr<GameMap> gamemap, observer_ptr<Entity> player)
    {
        map_    = gamemap;
        player_ = player;
    }

    void addEntity(observer_ptr<Entity> const entity)
    {
        lines_of_sight_.emplace(entity);
    }

    void deleteEntity(observer_ptr<Entity> const entity)
    {
        lines_of_sight_.erase(entity);
    }

    void assignPlayer(observer_ptr<Entity> const player) { player_ = player; }
    void assignMap(const observer_ptr<GameMap> map) { map_ = map; }

    void calculateLOS(observer_ptr<Entity> const entity,
                      observer_ptr<Entity> const target) noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Map unassigned");
        if (entity == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Caller unassigned");
        if (target == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Target unassigned");
        auto target_coord_ptr      = target->getComponent<Coordinates>();
        auto coord_ptr             = entity->getComponent<Coordinates>();
        auto los_ptr               = entity->getComponent<LOSComponent>();

        los_ptr->has_LOS_to_player = lineOfSightAlg(coord_ptr->x,
                                                    coord_ptr->y,
                                                    target_coord_ptr->x,
                                                    target_coord_ptr->y,
                                                    los_ptr->seeing_distance);
    }

    void calculateAllLinesOfSight() noexcept(false)
    {
        if (player_ == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Player unassigned");
        auto player_coords = player_->getComponent<Coordinates>();
        for (auto los_entity : lines_of_sight_)
        {
            if (los_entity->hasComponent<Coordinates>() == false)
            {
                continue;
            }
            auto coord_ptr = los_entity->getComponent<Coordinates>();
            auto los_ptr   = los_entity->getComponent<LOSComponent>();

            los_ptr->has_LOS_to_player =
                lineOfSightAlg(coord_ptr->x,
                               coord_ptr->y,
                               player_coords->x,
                               player_coords->y,
                               los_ptr->seeing_distance);
        }
    }

    bool isBlocked(short destX, short destY) const noexcept(false)
    {

        if (map_ == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Map unassigned");
        if (destX < 0 || destX >= G_MAP_WIDTH)
        {
            return true;
        }
        if (destY < 0 || destY > G_MAP_HEIGHT)
        {
            return true;
        }

        if (((*map_)[destX][destY].type & TileType::WALL) != TileType::NONE)
            return true;
        return false;
    }

    void visit(short destX, short destY)
    {
        if (map_ == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Map unassigned");
        if (destX < 0 || destX >= G_MAP_WIDTH)
        {
            return;
        }
        if (destY < 0 || destY > G_MAP_HEIGHT)
        {
            return;
        }
        visited_list_.emplace_back(destX, destY);
        (*map_)[destX][destY].type |= (TileType::MAPPED | TileType::VISIBLE);
    }

    void cleanVisited() noexcept(false)
    {
        if (map_ == nullptr)
            throw std::runtime_error("LOS_System: ERROR -> Map unassigned");
        for (auto &[x, y] : visited_list_)
        {
            (*map_)[x][y].type &= ~TileType::VISIBLE;
        }
        visited_list_.clear();
    }

    void updateData() noexcept(false) override
    {
        for (auto &entity : deletion_messages_)
        {
            deleteEntity(entity);
        }
        for (auto &entity : addition_messages_)
        {
            addEntity(entity);
        }

        calculateAllLinesOfSight();
        cleanVisited();
    }
    void readSystemMessages() override
    {
        for (auto &message : (*system_messages_)[SystemType::LINE_OF_SIGHT])
        {
            auto message_it = message.begin();
            auto action =
                std::any_cast<SystemAction::LINE_OF_SIGHT>(*message_it);
            ++message_it;
            switch (action)
            {
            case SystemAction::LINE_OF_SIGHT::ADD:
            {
                addition_messages_.emplace_back(
                    std::any_cast<observer_ptr<Entity>>(*message_it));
                break;
            }
            case SystemAction::LINE_OF_SIGHT::DELETE:
            {
                deletion_messages_.emplace_back(
                    std::any_cast<observer_ptr<Entity>>(*message_it));
                break;
            }
            case SystemAction::LINE_OF_SIGHT::CALCULATE:
            {
                auto caller = std::any_cast<observer_ptr<Entity>>(*message_it);
                ++message_it;
                auto target = std::any_cast<observer_ptr<Entity>>(*message_it);
                calculateLOS(caller, target);
                break;
            }
            case SystemAction::LINE_OF_SIGHT::CALCULATE_ALL:
            {
                calculateAllLinesOfSight();
            }
            }
        }
    }
    void clearSystemMessages() override
    {
        (*system_messages_)[SystemType::LINE_OF_SIGHT].clear();
        addition_messages_.clear();
        deletion_messages_.clear();
    }

    std::ostream &serialize(std::ostream &os) const override
    {
        os << SystemType::LINE_OF_SIGHT << ' ' << lines_of_sight_.size() << ' ';
        for (auto &entity : lines_of_sight_)
        {
            os << entity->getId() << ' ';
        }
        return os;
    }
    std::istream &deserialize(std::istream &is) override
    {
        std::size_t linesofsight_size{};
        is >> linesofsight_size;
        if (linesofsight_size == 0)
        {
            return is;
        }
        lines_of_sight_.clear();
        lines_of_sight_.reserve(linesofsight_size);
        std::list<uint32_t> entity_requests;
        uint32_t            temp{};
        for (std::size_t i = 0; i < linesofsight_size; i++)
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
    void loadEntities(std::list<observer_ptr<Entity>> entities) override
    {
        for (auto &entity : entities)
        {
            addEntity(entity);
        }
    }
};

#endif /*LOSSYSTEM_H*/
