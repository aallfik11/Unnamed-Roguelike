#ifndef ENTITY_H
#define ENTITY_H
#include "component.h"
#include <memory>
#include <algorithm>
#include <vector>
#include <typeinfo>

class Entity
{
    static uint32_t _max_id;
    uint32_t _id;
    std::vector<std::shared_ptr<Component>> _components;

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
            _components.emplace_back(std::shared_ptr<Component>(component));
        }
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
        _components.emplace_back(std::move(component));
    }

    void add_component(Component *component)
    {
        _components.emplace_back(std::shared_ptr<Component>(component));
    }

    template <class ComponentType>
    bool has_component()
    {
        return std::any_of(_components.begin(), _components.end(), [](std::shared_ptr<Component> &component)
                           { return (typeid(ComponentType) == typeid(*(component.get()))); });
    }

    template <class ComponentType>
    std::shared_ptr<ComponentType> get_component()
    {
        auto it = std::find_if(_components.begin(), _components.end(), [](std::shared_ptr<Component> &component)
                               { return (typeid(ComponentType) == typeid(*(component.get()))); });

        if (it == _components.end())
            return nullptr;

        return std::static_pointer_cast<ComponentType>(*it);
    }
};

uint32_t Entity::_max_id = 0;

#endif /*ENTITY_H*/