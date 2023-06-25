#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include "entitytypes.h"
#include "observerptr.h"
#include <istream>
#include <memory>
#include <ostream>
#include <typeindex>
#include <unordered_map>
#include <vector>

class EntityManager; // for friend declaration;
class Entity
{
    friend class EntityManager;
    using Components =
        std::unordered_map<std::type_index, std::unique_ptr<Component>>;

    static uint32_t max_id_;
    uint32_t        id_;
    Components      components_;

    Entity();

public:
    EntityType type;

    Entity(EntityType type);
    Entity(EntityType type, std::vector<Component *> &components);
    Entity(EntityType type, std::initializer_list<Component *> components);
    Entity(const Entity &entity);
    Entity(bool no_id_increment);
    Entity(Entity &&entity);

    uint32_t        getId() const;
    static uint32_t getMaxId();

    void addComponent(Component *component);
    void addComponent(std::unique_ptr<Component> &&component);
    void addComponent(std::unique_ptr<Component> &component);

    template <class ComponentType> void removeComponent()
    {
        components_.erase(typeid(ComponentType));
    }

    template <class ComponentType> bool hasComponent() const
    {
        return components_.contains(typeid(ComponentType));
    }

    template <class ComponentType> observer_ptr<ComponentType> getComponent()
    {
        if (auto it = components_.find(typeid(ComponentType));
            it != components_.end())
        {
            return static_cast<observer_ptr<ComponentType>>(it->second.get());
        }
        return nullptr;
    }

    template <class ComponentType>
    observer_ptr<const ComponentType> getConstComponent() const
    {
        if (auto it = components_.find(typeid(ComponentType));
            it != components_.end())
        {
            return static_cast<observer_ptr<const ComponentType>>(
                it->second.get());
        }
        return nullptr;
    }

    friend std::ostream &operator<<(std::ostream &os, const Entity &entity);
    friend std::istream &operator>>(std::istream &is, Entity &entity);

    static void resetMaxId();

    bool operator==(const Entity &e) const;
};
#endif /*ENTITY_H*/
