#ifndef POSITIONSYSTEM_H
#define POSITIONSYSTEM_H
#include "../components/coordinates.h"
#include "../directions.h"
#include "../entity.h"
#include "../entityhash.h"
#include "../entityholder.h"
#include "../system.h"
#include "../tile.h"
#include <cstdint>
#include <list>
#include <unordered_set>

class PositionSystem : public System, public EntityHolder
{
    using EntityId = uint32_t;

    using GameMap  = std::vector<std::vector<Tile>>;
    using Message  = std::tuple<observer_ptr<Entity>, uint16_t, uint16_t>;

    std::unordered_set<observer_ptr<Entity>> entity_positions_;
    std::unordered_set<EntityId>             entities_stored_;
    std::list<Message>                       pos_change_messages_;
    std::list<std::pair<uint16_t, uint16_t>> request_messages_;
    std::list<observer_ptr<Entity>>          addition_messages_;
    std::list<observer_ptr<Entity>>          removal_messages_;
    observer_ptr<GameMap>                    map_;

    /**
     * @brief Checks if the coordinates are within the map's bounds
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Coordinates are valid,
     * @return false - Coordinates are not valid
     */
    bool checkCoordinateValidity(uint16_t x, uint16_t y) const;

    /**
     * @brief Checks for potential collisions with walls and other entities
     *
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true - Collission,
     * @return false - No collision
     */
    bool checkCollision(uint16_t x, uint16_t y) const;

public:
    PositionSystem();
    PositionSystem(observer_ptr<GameMap> given_map);

    void assignMap(observer_ptr<GameMap> map);

    /**
     * @brief Updates given entities' position in the system
     *
     * @param entity Entity whose position will be updated
     * @param x Coordinate x
     * @param y Coordinate y
     * @return true Successfully updated the position,
     * @return false Position could not be updated
     */
    bool
    updatePosition(const observer_ptr<Entity> entity, uint16_t x, uint16_t y);

    /**
     * @brief A version of updatePosition which accepts enumerated type
     * Direction as an argument
     *
     * @param entity Entity whose position will be updated
     * @param direction Can be UP, DOWN, LEFT or RIGHT, the position is updated
     * accordingly
     * @return true - Successfuly updated the position,
     * @return false - Position could not be updated
     */
    bool updatePosition(const observer_ptr<Entity> entity, Direction direction);

    std::list<observer_ptr<Entity>> getEntitiesAtCoordinates(uint16_t x,
                                                             uint16_t y);

    /**
     * @brief Adds an entity to the system
     *:lua vim.lsp.buf.range_code_action()<cr>
     * @param entity
     */
    void addEntity(const observer_ptr<Entity> entity);

    /**
     * @brief Deletes entity with a given id, does nothing if no id is matched
     * to the argument
     *
     * @param entity
     */
    void deleteEntity(const observer_ptr<Entity> entity);

    Tile getTileAtCoordinates(uint16_t x, uint16_t y);

    void updateData() noexcept(false) override;
    void readSystemMessages() noexcept(false) override;
    void clearSystemMessages() override;
    void resetSystem() override;

    virtual std::ostream &serialize(std::ostream &os) const override;
    virtual std::istream &deserialize(std::istream &is) override;

    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*POSITIONSYSTEM*/
