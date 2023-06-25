#ifndef INVENTORY_UI_H
#define INVENTORY_UI_H
#include "../effect.h"
#include "../entity.h"
#include "../observerptr.h"
#include "../rarity.h"
#include "../systems/inventorysystem.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>
#include <list>
#include <string>

class InventoryUI
{
    bool             is_stats_open_;
    bool             is_desc_open_;
    InventorySystem &inv_sys_;

    inline ftxui::Color getColorByRarity(Rarity rarity) const;
    inline ftxui::Color getColorByEffect(Effect effect) const;

    inline std::string getEffectName(Effect effect) const;

    ftxui::MenuEntryOption getItemMenuEntry(observer_ptr<Entity> item);
    std::string            getEffectDuration(Effect                        key,
                                             observer_ptr<EffectComponent> effect);
    ftxui::Element         getItemStats(observer_ptr<Entity> item);
    void fixIndex(int &index, const std::size_t inventory_size);

public:
    InventoryUI(InventorySystem &inv_sys);

    void render(observer_ptr<Entity> player, ftxui::ScreenInteractive &scr);
};

#endif /*INVENTORY_UI_H*/
