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
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") {
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
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
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[3] == "default") {
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_talisman(const std::string& s) {
    auto split = tools::split(s, '_');
    auto talisman = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    if (talisman == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->remove_talisman();
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->remove_talisman();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->pick_talisman(ic, talisman);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->pick_talisman(ic, talisman);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_enchanted_item(const std::string& s) {
    auto split = tools::split(s, '_');
    auto enchanted = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    if (enchanted == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->remove_enchanted_item();
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->remove_enchanted_item();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->pick_enchanted_item(ic, enchanted);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->pick_enchanted_item(ic, enchanted);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_other_item(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto other = split[0];
    auto p = std::dynamic_pointer_cast<character_unit>(current);
    if (!is_checked) {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->remove_other(other);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->remove_other(other);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    } else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->pick_other(ic, other);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->pick_other(ic, other);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_banner(const std::string& s) {
    auto split = tools::split(s, '_');
    auto banner = split[0];
    if (banner == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_banner();
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_banner();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    } else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->pick_banner(ic, banner);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->pick_banner(ic, banner);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_arcane_item(const std::string& s) {
    auto split = tools::split(s, '_');
    auto arcane = split[0];
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    if (arcane == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->remove_arcane_item();
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->remove_arcane_item();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->pick_arcane_item(ic, arcane);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->pick_arcane_item(ic, arcane);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_mage_level(const std::string& s) {
    auto split = tools::split(s, '_');
    std::shared_ptr<mage_character_unit> p = std::dynamic_pointer_cast<mage_character_unit>(current);
    if (split[0] == "Default") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->reset_level();
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->reset_level();
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        auto level = static_cast<short>(std::stoi(split[1]));
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->change_level(level);
            army->update_on(p->id());
            return true;
        case InTree::ROSTER:
            p->change_level(level);
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
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        bool is_master = split[2] == "master";
        if (is_checked) {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(p->id());
                if (is_master) p->master().add_command_member(member);
                else p->slave().add_command_member(member);
                army->update_on(p->id());
                return true;
            case InTree::ROSTER:
                if (is_master) p->master().add_command_member(member);
                else p->slave().add_command_member(member);
                return false;
            default: throw std::runtime_error("No unit selected!");
            }
        }
        else {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(p->id());
                if (is_master) p->master().remove_command_member(member);
                else p->slave().remove_command_member(member);
                army->update_on(p->id());
                return true;
            case InTree::ROSTER:
                if (is_master) p->master().remove_command_member(member);
                else p->slave().remove_command_member(member);
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
            if (split[1] == "default") current->remove_oco_extra();
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[1] == "default") current->remove_oco_extra();
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[1] == "default") {
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[1] == "default") {
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_mc_extra(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (is_checked) {
        if (extra == "Battle Standard Bearer" && army->has_bsb())
            throw std::runtime_error("Army already contains a Battle Standard Bearer!");
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[1] == "default") {
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[1] == "default") {
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        if (extra == "Battle Standard Bearer") {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(current->id());
                current->remove_banner();
                army->update_on(current->id());
                break;
            case InTree::ROSTER:
                current->remove_banner();
                break;
            default: break;
            }
        }
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[1] == "default")
                current->remove_mc_extra(extra);
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            if (split[1] == "default")
                current->remove_mc_extra(extra);
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}
