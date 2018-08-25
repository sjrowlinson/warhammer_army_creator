#ifndef MAGIC_ITEM_BOX_H
#define MAGIC_ITEM_BOX_H

class ArmyCreator;

#include "army_list.h"
#include "army_maps.h"
#include "magic_item.h"
#include "option_selector.h"
#include "selection_tree.h"
#include "tools.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
#include <QString>
#include <QTabWidget>

class MagicItemBox {
private:
    ArmyCreator* creator;
    QTabWidget* box;
    std::shared_ptr<unit> current;
    InTree in_tree;
    ItemClass ic_selected;

    // generic method to do some common checks and book-keeping before
    // calling specific function according to item_type value
    QGroupBox* make_items_tab(const std::unordered_map<std::string, item>& items,
                              ItemType item_type);
    QGroupBox* make_weapons_tab(const std::vector<std::pair<std::string, item>>& opt_weapons);
    QGroupBox* make_armour_tab(const std::vector<std::pair<std::string, item>>& opt_armour);
    QGroupBox* make_talismans_tab(const std::vector<std::pair<std::string, item>>& opt_talismans);
    QGroupBox* make_enchanted_tab(const std::vector<std::pair<std::string, item>>& opt_enchanted);
    QGroupBox* make_arcane_tab(const std::vector<std::pair<std::string, item>>& opt_arcane);
    QGroupBox* make_banners_tab(const std::vector<std::pair<std::string, item>>& opt_banners);
    QGroupBox* make_other_tab(const std::vector<std::pair<std::string, item>>& opt_other);
public:
    MagicItemBox(ArmyCreator* creator_, QTabWidget* box_);
    ~MagicItemBox() = default;

    void reset(std::shared_ptr<unit> current_, InTree in_tree_);
    void reset_category(ItemClass ic_selected_);

    bool reinitialise(ItemType focus);
    void clear();
};

#endif // !MAGIC_ITEM_BOX_H
