#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include <memory>
#include <string>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <optional>
#include <typeinfo>
#include <type_traits>
#include <typeindex>

class Entity
{
    typedef std::unordered_map<std::type_index, std::shared_ptr<Component>> Components;
    uint64_t a;
    static uint32_t _max_id;
    uint32_t _id;
    Components _components;

public:
    Entity()
    {
        _id = _max_id;
        _max_id++;
    }

    Entity(std::vector<Component *> &components) : Entity()
    {
        for (auto &component : components)
        {
            _components.emplace(typeid(*component), std::shared_ptr<Component>(component));
        }
    }

    uint32_t get_id()
    {
        return _id;
    }

    // void update_components()
    // {
    //     for (auto &component : _components)
    //     {
    //         component.get()->update();
    //     }
    // }

    void add_component(std::shared_ptr<Component> &component)
    {
        _components.emplace(typeid(*(component.get())), component);
    }

    void add_component(Component *component)
    {
        _components.emplace(typeid(*component), std::shared_ptr<Component>(component));
    }

    template <class ComponentType>
    bool has_component()
    {
        return std::any_of(_components.begin(), _components.end(), [](const Components::value_type &component)
                           { return (typeid(ComponentType) == typeid(*(component.second.get()))); });
    }

    template <class ComponentType>
    std::shared_ptr<ComponentType> get_component()
    {
        // std::unordered_map<std::string, std::shared_ptr<Component>>::iterator
        auto it = std::find_if(_components.begin(), _components.end(),
                               [&](const Components::value_type &component)
                               { return (typeid(ComponentType) == typeid(*(component.second.get()))); });

        if (it == _components.end())
            return std::shared_ptr<ComponentType>(nullptr);

        return std::dynamic_pointer_cast<ComponentType>(it->second);
    }
};

uint32_t Entity::_max_id = 0;

#endif /*ENTITY_H*/