#ifndef OPTION_SELECTOR_H
#define OPTION_SELECTOR_H
#include "army_list.h"
#include "selection_tree.h"
#include "unit.h"

#include <memory>
#include <string>

enum class InTree {
    ROSTER,
    ARMY,
    NEITHER
};

class option_selector {
private:
    std::shared_ptr<selection_tree> st;
    std::shared_ptr<army_list> army;
    InTree in_tree;
    std::shared_ptr<unit> current;
public:
    explicit option_selector(
        std::shared_ptr<selection_tree> st_,
        std::shared_ptr<army_list> army_
    );

    // resets the option_selector instance to point
    // to a newly selected unit
    void reset(std::shared_ptr<unit> current_, InTree in_tree_);

    // selectors

    bool select_weapon(const std::string& s);
    bool select_armour(const std::string& s);
    bool select_talisman(const std::string& s);
    bool select_enchanted_item(const std::string& s);
    bool select_arcane_item(const std::string& s);
    bool select_other_item(const std::string& s, bool is_checked);
    bool select_mage_level(const std::string& s);
    bool select_mount(const std::string& s);
    bool select_command(const std::string& s, bool is_checked);
    bool select_oco_extra(const std::string& s);
    bool select_mc_extra(const std::string& s, bool is_checked);
};

#endif // !OPTION_SELECTOR_H
