#ifndef OPTION_SELECTOR_H
#define OPTION_SELECTOR_H

class ArmyCreator;

#include "army_list.h"
#include "enums.h"
#include "selection_tree.h"
#include "unit.h"

#include <memory>
#include <string>

#include <QLabel>

class option_selector {
private:
    std::shared_ptr<ArmyCreator> creator;
    std::shared_ptr<army_list> army;
    InTree in_tree;
    std::shared_ptr<unit> current;
    QLabel* budget_label;
public:
    explicit option_selector(
        std::shared_ptr<ArmyCreator> creator_,
        const std::shared_ptr<army_list>& army_
    );

    // resets the option_selector instance to point
    // to a newly selected unit
    void reset(const std::shared_ptr<unit>& current_, InTree in_tree_);

    // selectors
    void select_weapon(const std::string& weapon, WeaponType wt, ItemCategory ic, bool champion, bool master);
    void select_armour(const std::string& armour, ArmourType at, ItemCategory ic, bool champion, bool master);
    void select_talisman(const std::string& talisman, ItemCategory ic);
    void select_enchanted_item(const std::string& enchanted, ItemCategory ic);
    void select_arcane_item(const std::string& arcane, ItemCategory ic);
    void select_other_item(const std::string& other, ItemCategory ic, bool checked);
    void select_banner(const std::string& banner, ItemCategory ic);
    void select_mage_level(short level);
    void select_mage_lore(const std::string& lore, std::string to_remove);
    void select_mount(const std::string& mount);
    void select_mount_oco_extra(const std::string& extra);
    void select_mount_mc_extra(const std::string& extra, bool is_checked);
    void select_command(CommandGroup member, bool checked, bool master);
    void select_oco_extra(const std::string& extra, bool champion);
    void select_mc_extra(const std::string& extra, bool champion, bool checked);
};

#endif // !OPTION_SELECTOR_H
