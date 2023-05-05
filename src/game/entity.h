#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include "entitytypes.h"
#include <algorithm>
#include <memory>
#include <ranges>
#include <utility>
// #include <type_traits>
#include <istream>
#include <ostream>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Entity
{
    using Components =
        std::unordered_map<std::type_index, std::unique_ptr<Component>>;
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

    Entity(EntityType type, std::vector<Component *> &components) : Entity()
    {
        for (auto &component : components)
        {
            components_.emplace(typeid(*component),
                                std::unique_ptr<Component>(component));
        }
        this->type = type;
    }

    Entity(EntityType type, std::initializer_list<Component *> components)
        : Entity()
    {
        for (auto &component : components)
        {
            components_.emplace(typeid(*component),
                                std::unique_ptr<Component>(component));
        }
        this->type = type;
    }

    Entity(const Entity &entity) : Entity()
    {
        for (auto &component : entity.components_)
        {
            this->addComponent(component.second->clone());
        }
    }

    uint32_t        getId() const { return id_; }
    static uint32_t getMaxId() { return max_id_; }

    void addComponent(Component *component)
    {
        components_.emplace(typeid(*component), component);
    }

    void addComponent(std::unique_ptr<Component> &&component)
    {
        components_.emplace(typeid(*component), std::move(component));
    }
    void addComponent(std::unique_ptr<Component> &component)
    {
        components_.emplace(typeid(*component), std::move(component));
    }

    template <class ComponentType> void removeComponent()
    {
        components_.erase(typeid(ComponentType));
    }

    template <class ComponentType> bool hasComponent() const
    {
        return components_.contains(typeid(ComponentType));
    }

    template <class ComponentType> ComponentType *getComponent()
    {
        auto it = components_.find(typeid(ComponentType));

        if (it == components_.end())
            return nullptr;

        return static_cast<ComponentType *>(it->second.get());
    }

    friend std::ostream &operator<<(std::ostream       &os,
                                    const Entity *const entity);
    friend std::istream &operator>>(std::istream &is, Entity *const entity);

    static void resetMaxId() { max_id_ = 1; }
};

std::ostream &operator<<(std::ostream &os, const Entity *const entity) {
    os << entity->type << ' ' << entity->
}

uint32_t Entity::max_id_ = 1;

#endif /*ENTITY_H*/
