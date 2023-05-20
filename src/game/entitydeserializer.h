#ifndef ENTITYDESERIALIZER_H
#define ENTITYDESERIALIZER_H
#include "components/components_all.h"
#include "entity.h"
#include <functional>
#include <unordered_map>

static std::unique_ptr<Component> deserializeEntity(std::istream &is,
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

std::istream &operator>>(std::istream &is, Entity& entity)
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

#endif /*ENTITYDESERIALIZER_H*/
