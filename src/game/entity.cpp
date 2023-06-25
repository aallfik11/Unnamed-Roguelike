#include "entity.h"
#include "component.h"
#include "components/components_all.h"
#include "entitytypes.h"
#include "observerptr.h"
#include <algorithm>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <ranges>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <vector>

Entity::Entity()
{
    id_ = max_id_;
    max_id_++;
}

Entity::Entity(EntityType type) : Entity() { this->type = type; }

Entity::Entity(EntityType type, std::vector<Component *> &components) : Entity()
{
    for (auto &component : components)
    {
        components_.emplace(typeid(*component),
                            std::unique_ptr<Component>(component));
    }
    this->type = type;
}

Entity::Entity(EntityType type, std::initializer_list<Component *> components)
    : Entity()
{
    for (auto &component : components)
    {
        components_.emplace(typeid(*component),
                            std::unique_ptr<Component>(component));
    }
    this->type = type;
}

Entity::Entity(const Entity &entity) : Entity()
{
    this->type = entity.type;
    for (auto &component : entity.components_)
    {
        this->addComponent(component.second->clone());
    }
}

Entity::Entity(bool no_id_increment)
{
    this->id_ = 0;
    ++max_id_;
    this->type = EntityType::NONE;
}

Entity::Entity(Entity &&entity)
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

uint32_t Entity::getId() const { return id_; }
uint32_t Entity::getMaxId() { return max_id_; }

void Entity::addComponent(Component *component)
{
    components_.emplace(typeid(*component), component);
}

void Entity::addComponent(std::unique_ptr<Component> &&component)
{
    components_.emplace(typeid(*component), std::move(component));
}
void Entity::addComponent(std::unique_ptr<Component> &component)
{
    components_.emplace(typeid(*component), std::move(component));
}

void Entity::resetMaxId() { max_id_ = 1; }

bool Entity::operator==(const Entity &e) const
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

std::unique_ptr<Component> deserializeEntity(std::istream &is,
                                                    ComponentType type)
{
    Component *temp = nullptr;

    switch (type)
    {
    case ComponentType::AI:
        temp = new AIComponent;
        break;
    case ComponentType::AMULETSLOT:
        temp = new AmuletSlot;
        break;
    case ComponentType::ARMOR:
        temp = new ArmorComponent;
        break;
    case ComponentType::ARMORSLOT:
        temp = new ArmorSlot;
        break;
    case ComponentType::BUFF:
        temp = new BuffComponent;
        break;
    case ComponentType::COORDINATES:
        temp = new Coordinates;
        break;
    case ComponentType::CRIT:
        temp = new CritComponent;
        break;
    case ComponentType::DESCRIPTION:
        temp = new Description;
        break;
    case ComponentType::EFFECT:
        temp = new EffectComponent;
        break;
    case ComponentType::EXPERIENCE:
        temp = new ExperienceComponent;
        break;
    case ComponentType::HEALTH:
        temp = new Health;
        break;
    case ComponentType::HUNGER:
        temp = new HungerComponent;
        break;
    case ComponentType::INVENTORY:
        temp = new Inventory;
        break;
    case ComponentType::ITEM:
        temp = new ItemComponent;
        break;
    case ComponentType::LINEOFSIGHT:
        temp = new LOSComponent;
        break;
    case ComponentType::NAME:
        temp = new Name;
        break;
    case ComponentType::NAVMAP:
        temp = new NavMapComponent;
        break;
    case ComponentType::TILE:
        temp = new TileComponent;
        break;
    case ComponentType::WEAPON:
        temp = new WeaponComponent;
        break;
    case ComponentType::WEAPONSLOT:
        temp = new WeaponSlot;
        break;
    }
    is >> *temp;
    return std::unique_ptr<Component>(temp);
}

std::istream &operator>>(std::istream &is, Entity &entity)
{
    std::size_t number_of_components{};
    is >> entity.id_ >> entity.type >> number_of_components;
    ComponentType temp{};
    for (std::size_t i = 0; i < number_of_components; i++)
    {
        is >> temp;
        entity.addComponent(deserializeEntity(is, temp));
    }
    return is;
}
uint32_t Entity::max_id_ = 1;
