#include "option_selector.h"

option_selector::option_selector(
    const std::shared_ptr<army_list>& army_,
    QLabel* budget_label_
) : army(army_), in_tree(InTree::NEITHER), current(), budget_label(budget_label_) {}

void option_selector::reset(const std::shared_ptr<unit>& current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

void option_selector::update_budget_label() {
    if (current->is_character()) {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        std::string budget_str = "Magic: " +
                tools::points_str(p->handle_->magic_item_budget().points - p->magic_item_points());
        if (p->handle_->faction_items_handle() != nullptr) {
            budget_str += "  " + p->handle_->faction_items_handle()->first + ": " +
                tools::points_str(p->handle_->faction_item_budget().points - p->faction_item_points()) +
                "   Total: " + tools::points_str(p->handle_->total_item_budget().points - p->total_item_points());
        }
        budget_label->setText(QString::fromStdString(budget_str));
    }
}

bool option_selector::is_selection_magical(const std::string& selection) const {
    return (current->base()->common_items_handle() == nullptr ?
                false : current->base()->common_items_handle()->second.count(selection)) ||
            (current->base()->magic_items_handle() == nullptr ?
                 false : current->base()->magic_items_handle()->second.count(selection)) ||
            (current->base()->faction_items_handle() == nullptr ?
                 false : current->base()->faction_items_handle()->second.count(selection));
}

// selectors

void option_selector::select_weapon(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto weapon = split[0];
    std::string removed;
    if (weapon == "None") { // None radio button selected
        WeaponType wt;
        if (split[2] == "melee") wt = WeaponType::MELEE;
        else if (split[2] == "ranged") wt = WeaponType::BALLISTIC;
        else wt = WeaponType::NONE;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[3] == "default") removed = current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[3] == "default") removed = current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
    }
    else {
        const bool is_magical = is_selection_magical(weapon);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[2]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[3] == "default") {
                try { removed = current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { removed = current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[3] == "default") {
                try { removed = current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { removed = current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
        if (is_magical) {
            army->incr_item_tracker(weapon);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_armour(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto armour = split[0];
    std::string removed;
    if (armour == "None") {
        ArmourType at;
        if (split[2] == "armour") at = ArmourType::ARMOUR;
        else if (split[2] == "shield") at = ArmourType::SHIELD;
        else if (split[2] == "helmet") at = ArmourType::HELMET;
        else at = ArmourType::NONE;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[3] == "default") removed = current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[3] == "default") removed = current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
    }
    else {
        const bool is_magical = is_selection_magical(armour);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[2]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[3] == "default") {
                try { removed = current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { removed = current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[3] == "default") {
                try { removed = current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { removed = current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
        if (is_magical) {
            army->incr_item_tracker(armour);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_talisman(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto talisman = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (talisman == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->remove_talisman();
            army->update_on(p->id());
        } else removed = p->remove_talisman();
    }
    else {
        const bool is_magical = is_selection_magical(talisman);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->pick_talisman(ic, talisman);
            army->update_on(p->id());
        } else removed = p->pick_talisman(ic, talisman);
        if (is_magical) {
            army->incr_item_tracker(talisman);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_enchanted_item(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto enchanted = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (enchanted == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->remove_enchanted_item();
            army->update_on(p->id());
        } else removed = p->remove_enchanted_item();
    }
    else {
        const bool is_magical = is_selection_magical(enchanted);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->pick_enchanted_item(ic, enchanted);
            army->update_on(p->id());
        } else removed = p->pick_enchanted_item(ic, enchanted);
        if (is_magical) {
            army->incr_item_tracker(enchanted);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_other_item(const std::string& s, bool is_checked) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto other = split[0];
    auto p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (!is_checked) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->remove_other(other);
            army->update_on(p->id());
        } else removed = p->remove_other(other);
    } else {
        const bool is_magical = is_selection_magical(other);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->pick_other(ic, other);
            army->update_on(p->id());
        } else removed = p->pick_other(ic, other);
        if (is_magical) {
            army->incr_item_tracker(other);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_banner(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto banner = split[0];
    std::string removed;
    if (banner == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            removed = current->remove_banner();
            army->update_on(current->id());
        } else removed = current->remove_banner();
    } else {
        const bool is_magical = is_selection_magical(banner);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            removed = current->pick_banner(ic, banner);
            army->update_on(current->id());
        } else removed = current->pick_banner(ic, banner);
        if (is_magical) {
            army->incr_item_tracker(banner);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_arcane_item(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto arcane = split[0];
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    std::string removed;
    if (arcane == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->remove_arcane_item();
            army->update_on(p->id());
        } else removed = p->remove_arcane_item();
    }
    else {
        const bool is_magical = is_selection_magical(arcane);
        ItemCategory ic = static_cast<ItemCategory>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            removed = p->pick_arcane_item(ic, arcane);
            army->update_on(p->id());
        } else removed = p->pick_arcane_item(ic, arcane);
        if (is_magical) {
            army->incr_item_tracker(arcane);
            update_budget_label();
        }
    }
    if (!removed.empty() && is_selection_magical(removed)) {
        army->decr_item_tracker(removed);
        update_budget_label();
    }
}

void option_selector::select_mage_level(const std::string& s) {
    auto split = tools::split(s, '_');
    std::shared_ptr<mage_character_unit> p = std::dynamic_pointer_cast<mage_character_unit>(current);
    if (split[0] == "Default") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->reset_level();
            army->update_on(p->id());
        } else p->reset_level();
    }
    else {
        auto level = static_cast<short>(std::stoi(split[1]));
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->change_level(level);
            army->update_on(p->id());
        } else p->change_level(level);
    }
}

void option_selector::select_mage_lore(const std::string& s) {
    auto lore = tools::split(s, '_')[1];
    std::shared_ptr<mage_character_unit> p = std::dynamic_pointer_cast<mage_character_unit>(current);
    p->pick_lore(lore);
}

void option_selector::select_mount(const std::string& s) {
    auto split = tools::split(s, '_');
    auto mount = split[0];
    if (mount == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_mount();
            army->update_on(current->id());
        } else current->remove_mount();
    }
    else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->pick_mount(mount);
            army->update_on(current->id());
        } else current->pick_mount(mount);
    }
}

void option_selector::select_mount_oco_extra(const std::string& s) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (extra == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_mount_option(std::get<2>(current->mnt()).first, true);
            army->update_on(current->id());
        } else current->remove_mount_option(std::get<2>(current->mnt()).first, true);
    }
    else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            try { current->pick_mount_option(extra, true); }
            catch (const std::exception&) { throw; }
            army->update_on(current->id());
        } else {
            try { current->pick_mount_option(extra, true); }
            catch (const std::exception&) { throw; }
        }
    }
}

void option_selector::select_mount_mc_extra(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (is_checked) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            try { current->pick_mount_option(extra, false); }
            catch (const std::exception&) { throw; }
            army->update_on(current->id());
        } else {
            try { current->pick_mount_option(extra, false); }
            catch (const std::exception&) { throw; }
        }
    }
    else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_mount_option(extra, false);
            army->update_on(current->id());
        } else current->remove_mount_option(extra, false);
    }
}

void option_selector::select_command(const std::string& s, bool is_checked) {
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
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                p->add_command_member(member);
                army->update_on(p->id());
            } else p->add_command_member(member);
        }
        else {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                p->remove_command_member(member);
                army->update_on(p->id());
            } else p->remove_command_member(member);
        }
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        bool is_master = split[2] == "master";
        if (is_checked) {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                if (is_master) p->master().add_command_member(member);
                else p->slave().add_command_member(member);
                army->update_on(p->id());
            } else {
                if (is_master) p->master().add_command_member(member);
                else p->slave().add_command_member(member);
            }
        }
        else {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                if (is_master) p->master().remove_command_member(member);
                else p->slave().remove_command_member(member);
                army->update_on(p->id());
            } else {
                if (is_master) p->master().remove_command_member(member);
                else p->slave().remove_command_member(member);
            }
        }
        break;
    }
    default: break;
    }
}

void option_selector::select_oco_extra(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (extra == "None") {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[1] == "default") current->remove_oco_extra();
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[1] == "default") current->remove_oco_extra();
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
    }
    else {
        if (enum_convert::in_army_trees(in_tree)) {
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
        } else {
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
        }
    }
}

void option_selector::select_mc_extra(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (is_checked) {
        if (extra == "Battle Standard Bearer" && army->has_bsb())
            throw std::runtime_error("Army already contains a Battle Standard Bearer!");
        if (enum_convert::in_army_trees(in_tree)) {
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
        } else {
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
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (split[1] == "default")
                current->remove_mc_extra(extra);
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
        } else {
            if (split[1] == "default")
                current->remove_mc_extra(extra);
            else if (split[1] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
        }
    }
}
