#include "option_selector.h"
#include "armycreator.h"

option_selector::option_selector(
    std::shared_ptr<ArmyCreator> creator_,
    const std::shared_ptr<army_list>& army_
) : creator(creator_), army(army_), in_tree(InTree::NEITHER), current() {}

void option_selector::reset(const std::shared_ptr<unit>& current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

// selectors

void option_selector::select_weapon(const std::string& weapon, WeaponType wt, ItemCategory ic, bool champion, bool master) {
    (void)master;
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    if (weapon.empty()) { // None radio button selected => remove weapon
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            } else current->remove_weapon(wt);
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            } else current->remove_weapon(wt);
        }
    } else {
        const bool is_magical = ic == ItemCategory::COMMON || ic == ItemCategory::MAGIC || ic == ItemCategory::FACTION;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_weapon(ic, weapon); }
                catch (const std::exception&) { throw; }
            }
        }
        if (is_magical) creator->update_budget_label();

    }
    creator->update_budget_label();
}

void option_selector::select_armour(const std::string& armour, ArmourType at, ItemCategory ic, bool champion, bool master) {
    (void)master;
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    if (armour.empty()) { // None radio button selected => remove weapon
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            } else current->remove_armour(at);
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            } else current->remove_armour(at);
        }
    } else {
        const bool is_magical = ic == ItemCategory::COMMON || ic == ItemCategory::MAGIC || ic == ItemCategory::FACTION;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_armour(ic, armour); }
                catch (const std::exception&) { throw; }
            }
        }
        if (is_magical) creator->update_budget_label();

    }
    creator->update_budget_label();
}

void option_selector::select_talisman(const std::string& talisman, ItemCategory ic) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    if (talisman.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->remove_talisman();
            army->update_on(p->id());
        } else p->remove_talisman();
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->pick_talisman(ic, talisman);
            army->update_on(p->id());
        } else p->pick_talisman(ic, talisman);
        creator->update_budget_label();
    }
    creator->update_budget_label();
}

void option_selector::select_enchanted_item(const std::string& enchanted, ItemCategory ic) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    if (enchanted.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->remove_enchanted_item();
            army->update_on(p->id());
        } else p->remove_enchanted_item();
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->pick_enchanted_item(ic, enchanted);
            army->update_on(p->id());
        } else p->pick_enchanted_item(ic, enchanted);
        creator->update_budget_label();
    }
    creator->update_budget_label();
}

void option_selector::select_other_item(const std::string& other, ItemCategory ic, bool checked) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto p = std::dynamic_pointer_cast<character_unit>(current);
    if (!checked) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->remove_magic_extra(other);
            army->update_on(p->id());
        } else p->remove_magic_extra(other);
    } else {
        const bool is_magical = ic == ItemCategory::COMMON || ic == ItemCategory::MAGIC || ic == ItemCategory::FACTION;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->pick_magic_extra(ic, other);
            army->update_on(p->id());
        } else p->pick_magic_extra(ic, other);
        if (is_magical) creator->update_budget_label();

    }
    creator->update_budget_label();
}

void option_selector::select_banner(const std::string& banner, ItemCategory ic) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    if (banner.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_banner();
            army->update_on(current->id());
        } else current->remove_banner();
    } else {
        const bool is_magical = ic == ItemCategory::COMMON || ic == ItemCategory::MAGIC || ic == ItemCategory::FACTION;
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->pick_banner(ic, banner);
            army->update_on(current->id());
        } else current->pick_banner(ic, banner);
        if (is_magical) creator->update_budget_label();
    }
    creator->update_budget_label();
}

void option_selector::select_arcane_item(const std::string& arcane, ItemCategory ic) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    if (arcane.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->remove_arcane_item();
            army->update_on(p->id());
        } else p->remove_arcane_item();
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->pick_arcane_item(ic, arcane);
            army->update_on(p->id());
        } else p->pick_arcane_item(ic, arcane);
        creator->update_budget_label();
    }
    creator->update_budget_label();
}

void option_selector::select_mage_level(short level) {
    std::shared_ptr<mage_character_unit> p = std::dynamic_pointer_cast<mage_character_unit>(current);
    if (level == p->handle->mage_level()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->reset_level();
            army->update_on(p->id());
        } else p->reset_level();
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(p->id());
            p->change_level(level);
            army->update_on(p->id());
        } else p->change_level(level);
    }
}

void option_selector::select_mage_lore(const std::string& lore, std::string to_remove) {
    std::shared_ptr<mage_character_unit> p = std::dynamic_pointer_cast<mage_character_unit>(current);
    // erase a lore
    if (!to_remove.empty()) {
        p->remove_lore(to_remove);
        // if lore to remove same as lore to pick then this indicates a check-box
        // being unchecked - so return immediately without repicking the same lore
        if (to_remove == lore) return;
    }
    // pick a lore
    p->pick_lore(lore);
}

void option_selector::select_mount(const std::string& mount) {
    if (mount.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_mount();
            army->update_on(current->id());
        } else current->remove_mount();
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->pick_mount(mount);
            army->update_on(current->id());
        } else current->pick_mount(mount);
    }
}

void option_selector::select_mount_oco_extra(const std::string& extra) {
    if (extra.empty()) {
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

void option_selector::select_mount_mc_extra(const std::string& extra, bool is_checked) {
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
    } else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            current->remove_mount_option(extra, false);
            army->update_on(current->id());
        } else current->remove_mount_option(extra, false);
    }
}

void option_selector::select_command(CommandGroup member, bool checked, bool master) {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (checked) {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                p->add_command_member(member);
                army->update_on(p->id());
            } else p->add_command_member(member);
        } else {
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
        if (checked) {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                if (master) p->add_master_command_member(member);
                else p->add_slave_command_member(member);
                army->update_on(p->id());
            } else {
                if (master) p->add_master_command_member(member);
                else p->add_slave_command_member(member);
            }
        }
        else {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(p->id());
                if (master) p->remove_master_command_member(member);
                else p->remove_slave_command_member(member);
                army->update_on(p->id());
            } else {
                if (master) p->remove_master_command_member(member);
                else p->remove_slave_command_member(member);
            }
        }
        break;
    }
    default: break;
    }
}

void option_selector::select_oco_extra(const std::string& extra, bool champion) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    if (extra.empty()) {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            else current->remove_oco_extra();
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_oco_extra();
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            else current->remove_oco_extra();
        }
    }
    else {
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_oco_extra(extra); }
                catch (const std::exception&) { throw; }
            }
        }
    }
}

void option_selector::select_mc_extra(const std::string& extra, bool champion, bool checked) {
    if (checked) {
        if (extra == "Battle Standard Bearer" && army->has_bsb())
            throw std::runtime_error("Army already contains a Battle Standard Bearer!");
        if (enum_convert::in_army_trees(in_tree)) {
            army->take_snapshot_of(current->id());
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) { throw; }
            }
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) {
                    current->switch_model_select(ModelSelect::DEFAULT);
                    throw;
                }
                current->switch_model_select(ModelSelect::DEFAULT);
            } else {
                try { current->pick_mc_extra(extra); }
                catch (const std::exception&) { throw; }
            }
        }
    } else {
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
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            else current->remove_mc_extra(extra);
            army->update_on(current->id());
        } else {
            if (champion) {
                current->switch_model_select(ModelSelect::CHAMPION);
                current->remove_mc_extra(extra);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            else current->remove_mc_extra(extra);
        }
    }
}
