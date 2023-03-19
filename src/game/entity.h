#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include <memory>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <optional> //I wanna experiment with it later for get_component function
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
        // return std::any_of(_components.begin(), _components.end(), [](const Components::value_type &component)
        //                    { return (typeid(ComponentType) == typeid(*(component.second.get()))); });
        return _components.contains(typeid(ComponentType));
    }

    template <class ComponentType>
    std::shared_ptr<ComponentType> get_component()
    {
        // auto it = std::find_if(_components.begin(), _components.end(),
        //                        [&](const Components::value_type &component)
        //                        { return (typeid(ComponentType) == typeid(*(component.second.get()))); });
        auto it = _components.find(typeid(ComponentType));

        if (it == _components.end())
            return std::shared_ptr<ComponentType>(nullptr);

        return std::static_pointer_cast<ComponentType>(it->second);
    }

    static void reset_max_id()
    {
        _max_id = 0;
    }
};

uint32_t Entity::_max_id = 0;

#endif /*ENTITY_H*/