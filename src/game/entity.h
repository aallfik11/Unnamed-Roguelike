#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include <algorithm>
#include <memory>
#include <optional> //I wanna experiment with it later for get_component function
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

class Entity
{
    typedef std::unordered_map<std::type_index, std::shared_ptr<Component>> Components;
    static uint32_t _max_id;
    uint32_t _id;
    Components _components;

public:
    Entity()
    {
        _id = _max_id;
        _max_id++;
    }

    Entity(std::vector<Component *> components) : Entity()
    {
        for (auto &component : components)
        {
            _components.emplace(typeid(*component), std::shared_ptr<Component>(component));
        }
    }

    uint32_t getId()
    {
        return _id;
    }

    void addComponent(const std::shared_ptr<Component> &component)
    {
        _components.emplace(typeid(*(component.get())), component);
    }

    void addComponent(Component *component)
    {
        _components.emplace(typeid(*component), std::shared_ptr<Component>(component));
    }

    template <class ComponentType>
    void removeComponent()
    {
        _components.erase(typeid(ComponentType));
    }

    template <class ComponentType>
    bool hasComponent()
    {
        // return std::any_of(_components.begin(), _components.end(), [](const Components::value_type &component)
        //                    { return (typeid(ComponentType) == typeid(*(component.second.get()))); });
        return _components.contains(typeid(ComponentType));
    }

    template <class ComponentType>
    std::shared_ptr<ComponentType> getComponent()
    {
        // auto it = std::find_if(_components.begin(), _components.end(),
        //                        [&](const Components::value_type &component)
        //                        { return (typeid(ComponentType) == typeid(*(component.second.get()))); });
        auto it = _components.find(typeid(ComponentType));

        if (it == _components.end())
            return std::shared_ptr<ComponentType>(nullptr);

        return std::static_pointer_cast<ComponentType>(it->second);
    }

    static void resetMaxId()
    {
        _max_id = 0;
    }
};

uint32_t Entity::_max_id = 0;

#endif /*ENTITY_H*/