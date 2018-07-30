#include "option_selector.h"

option_selector::option_selector(
    std::shared_ptr<selection_tree> st_,
    std::shared_ptr<army_list> army_
) : st(st_), army(army_), in_tree(InTree::NEITHER), current() {}

void option_selector::reset(std::shared_ptr<unit> current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

// selectors

bool option_selector::select_weapon(const std::string& s) {
    auto split = tools::split(s, '_');
    auto weapon = split[0];
    if (weapon == "None") { // None radio button selected
        WeaponType wt;
        if (split[2] == "melee") wt = WeaponType::MELEE;
        else if (split[2] == "ballistic") wt = WeaponType::BALLISTIC;
        else wt = WeaponType::NONE;
        switch (current->base_unit_type()) {
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(current->id());
                p->remove_weapon(wt);
                army->update_on(current->id());
                return true;
            case InTree::ROSTER:
                p->remove_weapon(wt);
                return false;
            default: throw std::runtime_error("No unit selected, cannot assign weapon!");
            }
        }
        case BaseUnitType::MAGE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
            if (in_tree == InTree::ROSTER) {
                int id = current->id();
                army->take_snapshot_of(id);
                p->remove_weapon(wt);
                army->update_on(id);
                return true;
            }
        }
        }
    }
    return false;
}