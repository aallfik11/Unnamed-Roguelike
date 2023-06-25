#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H
#include "../component.h"
#include "../entity.h"
#include "../entityholder.h"
#include "../entitytypes.h"
#include "../system.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

class EntityManager : public System
{
    using EntityHashmap = std::unordered_map<uint32_t, std::unique_ptr<Entity>>;
    using Request = std::pair<observer_ptr<EntityHolder>, std::list<uint32_t>>;
    EntityHashmap                   entities_;
    std::list<uint32_t>             purge_list_;
    std::list<observer_ptr<Entity>> add_list_;
    std::list<Request>              request_list_;

public:
    uint32_t createEntity(const EntityType         type,
                          std::vector<Component *> components);

    void addEntity(observer_ptr<Entity> entity);

    observer_ptr<Entity> getEntity(const uint32_t entity_id) const;

    EntityHashmap &getAllEntities();

    void markForDeletion(const uint32_t id);
    void unmarkForDeletion(const uint32_t id);

    void purge();

    void handleRequests();

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
    void resetSystem() override;

    void hardReset(); // use carefully

    std::ostream &serialize(std::ostream &os) const override;
    std::istream &deserialize(std::istream &is) override;

    // DEBUG
    bool operator==(const EntityManager &e);
};

#endif /*ENTITYMANAGER*/
