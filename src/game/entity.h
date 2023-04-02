#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include <algorithm>
#include <memory>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Entity
{
    using Components =
        std::unordered_map<std::type_index, std::shared_ptr<Component>>;
    static uint32_t max_id_;
    uint32_t        id_;
    Components      components_;

public:
    Entity()
    {
        id_ = max_id_;
        max_id_++;
    }

    Entity(const std::vector<Component *> &components) : Entity()
    {
        for (auto &component : components)
        {
            components_.emplace(typeid(*component),
                                std::shared_ptr<Component>(component));
        }
    }

    uint32_t getId() { return id_; }

    void addComponent(const std::shared_ptr<Component> &component)
    {
        components_.emplace(typeid(*(component.get())), component);
    }

    void addComponent(Component *component)
    {
        components_.emplace(typeid(*component),
                            std::shared_ptr<Component>(component));
    }

    template <class ComponentType> void removeComponent()
    {
        components_.erase(typeid(ComponentType));
    }

    template <class ComponentType> bool hasComponent()
    {
        return components_.contains(typeid(ComponentType));
    }

    template <class ComponentType> std::shared_ptr<ComponentType> getComponent()
    {
        auto it = components_.find(typeid(ComponentType));

        if (it == components_.end())
            return std::shared_ptr<ComponentType>(nullptr);

        return std::static_pointer_cast<ComponentType>(it->second);
    }

    static void resetMaxId() { max_id_ = 0; }
};

uint32_t Entity::max_id_ = 0;

#endif /*ENTITY_H*/