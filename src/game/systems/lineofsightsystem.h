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
                        uint16_t seeing_distance) const;

public:
    LOS_System();
    LOS_System(observer_ptr<GameMap> gamemap, observer_ptr<Entity> player);

    void addEntity(observer_ptr<Entity> const entity);

    void deleteEntity(observer_ptr<Entity> const entity);

    void assignPlayer(observer_ptr<Entity> const player);

    void assignMap(const observer_ptr<GameMap> map);

    void calculateLOS(observer_ptr<Entity> const entity,
                      observer_ptr<Entity> const target) noexcept(false);

    void calculateAllLinesOfSight() noexcept(false);

    bool isBlocked(short destX, short destY) const noexcept(false);

    void visit(short destX, short destY);

    void cleanVisited() noexcept(false);

    void updateData() noexcept(false) override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;
    void loadEntities(std::list<observer_ptr<Entity>> entities) override;
};

#endif /*LOSSYSTEM_H*/
