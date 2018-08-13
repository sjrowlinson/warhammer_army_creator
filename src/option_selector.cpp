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
        else if (split[2] == "ranged") wt = WeaponType::BALLISTIC;
        else wt = WeaponType::NONE;
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[2]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") {
                try {
                    current->pick_weapon(ic, weapon);
                }
                catch (const std::invalid_argument&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try {
                    current->pick_weapon(ic, weapon);
                }
                catch (const std::exception&) { throw; }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") {
                try {
                    current->pick_weapon(ic, weapon);
                }
                catch (const std::invalid_argument&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try {
                    current->pick_weapon(ic, weapon);
                }
                catch (const std::exception&) { throw; }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_armour(const std::string& s) {
    auto split = tools::split(s, '_');
    auto armour = split[0];
    if (armour == "None") {
        ArmourType at;
        if (split[2] == "armour") at = ArmourType::ARMOUR;
        else if (split[2] == "shield") at = ArmourType::SHIELD;
        else if (split[2] == "helmet") at = ArmourType::HELMET;
        else at = ArmourType::NONE;
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[2]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") {
                try {
                    current->pick_armour(ic, armour);
                } catch (const std::invalid_argument&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try {
                    current->pick_armour(ic, armour);
                } catch (const std::exception&) { throw; }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") {
                try {
                    current->pick_armour(ic, armour);
                } catch (const std::invalid_argument&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try {
                    current->pick_armour(ic, armour);
                } catch (const std::exception&) { throw; }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_mount(const std::string& s) {
    auto split = tools::split(s, '_');
    auto mount = split[0];
    if (mount == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_mount();
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_mount();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->pick_mount(mount);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->pick_mount(mount);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_command(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto name = split[0];
    CommandGroup member;
    if (split[1] == "m") member = CommandGroup::MUSICIAN;
    else if (split[1] == "c") member = CommandGroup::CHAMPION;
    else if (split[1] == "sb") member = CommandGroup::STANDARD_BEARER;
    else throw std::invalid_argument("Unrecognised command group member type!");
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (is_checked) {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(p->id());
                p->add_command_member(member);
                army->update_on(p->id());
                return true;
            case InTree::ROSTER:
                p->add_command_member(member);
                return false;
            default: throw std::runtime_error("No unit selected!");
            }
        }
        else {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(p->id());
                p->remove_command_member(member);
                army->update_on(p->id());
                return true;
            case InTree::ROSTER:
                p->remove_command_member(member);
                return false;
            default: throw std::runtime_error("No unit selected!");
            }
        }
    }
    default: return false;
    }
}

bool option_selector::select_oco_extra(const std::string& s) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (extra == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_oco_extra();
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_oco_extra();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->pick_oco_extra(extra);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->pick_oco_extra(extra);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_mc_extra(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (is_checked) {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->pick_mc_extra(extra);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->pick_mc_extra(extra);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_mc_extra(extra);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_mc_extra(extra);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}
