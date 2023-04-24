#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include "entitytypes.h"
#include <algorithm>
#include <memory>
#include <ranges>
// #include <type_traits>
// #include <typeindex>
// #include <typeinfo>
#include <unordered_set>
#include <vector>

class Entity
{
    using Components = std::unordered_set<std::unique_ptr<Component>>;
    static uint32_t max_id_;
    uint32_t        id_;

    Components components_;

    Entity()
    {
        id_ = max_id_;
        max_id_++;
    }

public:
    EntityType type;

    Entity(EntityType type) : Entity() { this->type = type; }

    Entity(EntityType type, std::initializer_list<Component *> components)
        : Entity()
    {
        for (auto &component : components)
        {
            components_.emplace(component);
        }
        this->type = type;
    }

    // Entity(const std::shared_ptr<Entity> &entity_ptr)
    // {
    //     for (auto &component : entity_ptr->components_)
    //     {
    //         this->addComponent(component.second->clone());
    //     }
    // }

    Entity(const Entity &entity) : Entity()
    {
        for (auto &component : entity.components_)
        {
            this->addComponent(component->clone());
        }
    }

    uint32_t getId() const { return id_; }

    // void addComponent(const std::shared_ptr<Component> &component)
    // {
    //     components_[typeid(*(component.get()))] = component;
    // }

    void addComponent(Component *component) { components_.emplace(component); }

    void addComponent(std::unique_ptr<Component> &&component)
    {
        components_.emplace(std::move(component));
    }

    // template <class ComponentType> void removeComponent()
    // {
    //     components_.erase(std::unique_ptr<ComponentType>);
    // }

    // template <class ComponentType> bool hasComponent() const
    // {
    //     return components_.contains(std::unique_ptr<ComponentType>);
    // }

    // template <class ComponentType> ComponentType *getComponent()
    // {
    //     auto it = components_.find(std::unique_ptr<ComponentType>);

    //     if (it == components_.end())
    //         return nullptr;

    //     return static_cast<ComponentType *>(it->second.get());
    // }

    static void resetMaxId() { max_id_ = 0; }
};

uint32_t Entity::max_id_ = 0;

#endif /*ENTITY_H*/