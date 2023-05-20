#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include "entitytypes.h"
#include "observerptr.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <ranges>
#include <utility>
// #include <type_traits>
#include <istream>
#include <map>
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

    Entity(bool no_id_increment)
    {
        this->id_ = 0;
        ++max_id_;
        this->type = EntityType::NONE;
    }

    Entity(Entity &&entity)
    {
        for (auto &component : entity.components_)
        {
            this->addComponent(component.second->clone());
        }
        this->type = entity.type;
        this->id_  = entity.id_;

        entity.components_.clear();
        entity.type = EntityType::NONE;
        entity.id_  = 0;
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

    friend std::ostream &operator<<(std::ostream &os, const Entity &entity);
    friend std::istream &operator>>(std::istream &is, Entity &entity);

    static void resetMaxId() { max_id_ = 1; }

    bool operator==(const Entity &e) const
    {
        for (auto &[type_id, component_ptr] : this->components_)
        {
            if (e.components_.contains(type_id) == false)
            {
                return false;
            }
            if (*component_ptr != *(e.components_.at(type_id)))
                return false;
        }
        return (this->type == e.type && this->id_ == e.id_);
    }
};

std::ostream &operator<<(std::ostream &os, const Entity &entity)
{
    os << entity.id_ << ' ' << entity.type << ' ' << entity.components_.size()
       << ' ';
    for (const auto &[key, component] : entity.components_)
    {
        os << *component;
    }
    return os;

    // DEBUG
}

uint32_t Entity::max_id_ = 1;

#endif /*ENTITY_H*/
