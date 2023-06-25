#ifndef INVENTORYSYSTEM_H
#define INVENTORYSYSTEM_H
#include "../components/buffcomponent.h"
#include "../entity.h"
#include "../system.h"
#include <istream>
#include <list>
#include <ostream>

class InventorySystem : public System
{

    using Inv = std::list<observer_ptr<Entity>>;
    std::list<std::pair<observer_ptr<Entity>, std::list<observer_ptr<Entity>>>>
                                                         addition_messages_;
    std::list<std::pair<observer_ptr<Entity>, uint32_t>> drop_messages_;
    std::list<std::pair<observer_ptr<Entity>, observer_ptr<Entity>>>
                                                         transfer_messages_;
    std::list<std::pair<observer_ptr<Entity>, uint32_t>> usage_messages_;

    Inv::iterator iterateToItem(Inv &inventory, uint32_t index);

    void addEquipmentBuff(const observer_ptr<BuffComponent>       caller_buffs,
                          const observer_ptr<const BuffComponent> item_buffs);

    void
    removeEquipmentBuff(const observer_ptr<BuffComponent>       caller_buffs,
                        const observer_ptr<const BuffComponent> item_buffs);

    bool stackItem(Inv &inventory, const observer_ptr<Entity> item);

    bool equip(const observer_ptr<Entity> caller, const Inv::iterator &item_it);

    void consume(const observer_ptr<Entity> caller,
                 const Inv::iterator       &item_it);

public:
    void addToInventory(const observer_ptr<Entity>                  caller,
                        std::initializer_list<observer_ptr<Entity>> items);
    void addToInventory(const observer_ptr<Entity>             caller,
                        const std::list<observer_ptr<Entity>> &items);

    observer_ptr<Entity> dropFromInventory(const observer_ptr<Entity> caller,
                                           const uint32_t             index,
                                           const bool add_to_pos = true);
    observer_ptr<Entity> dropFromInventory(const observer_ptr<Entity> caller,
                                           const Inv::iterator       &index,
                                           const bool add_to_pos = true);

    void dropAllItems(observer_ptr<Entity> caller);

    void useItem(const observer_ptr<Entity> caller, const uint32_t index);
    void useItem(const observer_ptr<Entity> caller, const Inv::iterator &index);

    void updateData() override;
    void readSystemMessages() override;
    void clearSystemMessages() override;
};
#endif /*INVENTORYSYSTEM_H*/
