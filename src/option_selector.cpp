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

void option_selector::item_limit_check(bool is_magic, ItemClass ic, const std::string& s) {
    if (!is_magic) return;
    std::unordered_map<std::string, item>::const_iterator item_it;
    switch (ic) {
    case ItemClass::COMMON:
        item_it = current->base()->common_items_handle()->second.find(s);
        break;
    case ItemClass::MAGIC:
        item_it = current->base()->magic_items_handle()->second.find(s);
        break;
    case ItemClass::FACTION:
        item_it = current->base()->faction_items_handle()->second.find(s);
        break;
    default: break;
    }
    auto itm_search = army->item_track_map().find(s);
    if (itm_search != army->item_track_map().cend()) {
        if (!(itm_search->second < item_it->second.limit)) {
            std::string msg = "Cannot have more than " + std::to_string(item_it->second.limit)
                    + ' ' + s + " items in a single army!";
            throw std::runtime_error(msg);
        }
    }
}

bool option_selector::select_weapon(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto weapon = split[0];
    std::string removed;
    if (weapon == "None") { // None radio button selected
        WeaponType wt;
        if (split[2] == "melee") wt = WeaponType::MELEE;
        else if (split[2] == "ranged") wt = WeaponType::BALLISTIC;
        else wt = WeaponType::NONE;
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") removed = current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            break;
        case InTree::ROSTER:
            if (split[3] == "default") removed = current->remove_weapon(wt);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_weapon(wt);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        const bool is_magical = current->base()->common_items_handle()->second.count(weapon) ||
                    current->base()->magic_items_handle()->second.count(weapon) ||
                    current->base()->faction_items_handle()->second.count(weapon);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[2]));
        try { item_limit_check(is_magical, ic, weapon); }
        catch (const std::runtime_error&) { throw; }
        switch (in_tree) {
        case InTree::ARMY:
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
            break;
        case InTree::ROSTER:
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
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(weapon);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_armour(const std::string& s) {
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
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            if (split[3] == "default") removed = current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            army->update_on(current->id());
            break;
        case InTree::ROSTER:
            if (split[3] == "default") removed = current->remove_armour(at);
            else if (split[3] == "champion") {
                current->switch_model_select(ModelSelect::CHAMPION);
                removed = current->remove_armour(at);
                current->switch_model_select(ModelSelect::DEFAULT);
            }
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        const bool is_magical = current->base()->common_items_handle()->second.count(armour) ||
                    current->base()->magic_items_handle()->second.count(armour) ||
                    current->base()->faction_items_handle()->second.count(armour);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[2]));
        try { item_limit_check(is_magical, ic, armour); }
        catch (const std::runtime_error&) { throw; }
        switch (in_tree) {
        case InTree::ARMY:
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
            break;
        case InTree::ROSTER:
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
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(armour);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_talisman(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto talisman = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (talisman == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->remove_talisman();
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->remove_talisman();
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        const bool is_magical = current->base()->common_items_handle()->second.count(talisman) ||
                    current->base()->magic_items_handle()->second.count(talisman) ||
                    current->base()->faction_items_handle()->second.count(talisman);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        try { item_limit_check(is_magical, ic, talisman); }
        catch (const std::runtime_error& ) { throw; }
        std::string removed;
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->pick_talisman(ic, talisman);
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->pick_talisman(ic, talisman);
            if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
                    current->base()->magic_items_handle()->second.count(removed) ||
                    current->base()->faction_items_handle()->second.count(removed)))
                army->decr_item_tracker(removed);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(talisman);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_enchanted_item(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto enchanted = split[0];
    std::shared_ptr<character_unit> p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (enchanted == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->remove_enchanted_item();
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->remove_enchanted_item();
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        const bool is_magical = current->base()->common_items_handle()->second.count(enchanted) ||
                    current->base()->magic_items_handle()->second.count(enchanted) ||
                    current->base()->faction_items_handle()->second.count(enchanted);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        try { item_limit_check(is_magical, ic, enchanted); }
        catch (const std::runtime_error& ) { throw; }
        std::string removed;
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            p->pick_enchanted_item(ic, enchanted);
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            p->pick_enchanted_item(ic, enchanted);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(enchanted);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_other_item(const std::string& s, bool is_checked) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto other = split[0];
    auto p = std::dynamic_pointer_cast<character_unit>(current);
    std::string removed;
    if (!is_checked) {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->remove_other(other);
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->remove_other(other);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    } else {
        const bool is_magical = current->base()->common_items_handle()->second.count(other) ||
                    current->base()->magic_items_handle()->second.count(other) ||
                    current->base()->faction_items_handle()->second.count(other);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        try { item_limit_check(is_magical, ic, other); }
        catch (const std::runtime_error& ) { throw; }
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->pick_other(ic, other);
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->pick_other(ic, other);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(other);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_banner(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto banner = split[0];
    std::string removed;
    if (banner == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            removed = current->remove_banner();
            army->update_on(current->id());
            break;
        case InTree::ROSTER:
            removed = current->remove_banner();
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    } else {
        const bool is_magical = current->base()->common_items_handle()->second.count(banner) ||
                    current->base()->magic_items_handle()->second.count(banner) ||
                    current->base()->faction_items_handle()->second.count(banner);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        try { item_limit_check(is_magical, ic, banner); }
        catch (const std::runtime_error& ) { throw; }
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            removed = current->pick_banner(ic, banner);
            army->update_on(current->id());
            break;
        case InTree::ROSTER:
            removed = current->pick_banner(ic, banner);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(banner);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
}

bool option_selector::select_arcane_item(const std::string& s) {
    if (current == nullptr) throw std::runtime_error("No unit selected!");
    auto split = tools::split(s, '_');
    auto arcane = split[0];
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    std::string removed;
    if (arcane == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->remove_arcane_item();
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->remove_arcane_item();
            break;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        const bool is_magical = current->base()->common_items_handle()->second.count(arcane) ||
                    current->base()->magic_items_handle()->second.count(arcane) ||
                    current->base()->faction_items_handle()->second.count(arcane);
        ItemClass ic = static_cast<ItemClass>(std::stoi(split[1]));
        try { item_limit_check(is_magical, ic, arcane); }
        catch (const std::runtime_error& ) { throw; }
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(p->id());
            removed = p->pick_arcane_item(ic, arcane);
            army->update_on(p->id());
            break;
        case InTree::ROSTER:
            removed = p->pick_arcane_item(ic, arcane);
            break;
        default: throw std::runtime_error("No unit selected!");
        }
        if (is_magical) army->incr_item_tracker(arcane);
    }
    if (!removed.empty() && (current->base()->common_items_handle()->second.count(removed) ||
            current->base()->magic_items_handle()->second.count(removed) ||
            current->base()->faction_items_handle()->second.count(removed)))
        army->decr_item_tracker(removed);
    return in_tree == InTree::ARMY;
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

bool option_selector::select_mount_oco_extra(const std::string& s) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (extra == "None") {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_mount_option(std::get<2>(current->mnt()).first, true);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_mount_option(std::get<2>(current->mnt()).first, true);
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            try { current->pick_mount_option(extra, true); }
            catch (const std::exception&) { throw; }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            try { current->pick_mount_option(extra, true); }
            catch (const std::exception&) { throw; }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
}

bool option_selector::select_mount_mc_extra(const std::string& s, bool is_checked) {
    auto split = tools::split(s, '_');
    auto extra = split[0];
    if (is_checked) {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            try { current->pick_mount_option(extra, false); }
            catch (const std::exception&) { throw; }
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            try { current->pick_mount_option(extra, false); }
            catch (const std::exception&) { throw; }
            return false;
        default: throw std::runtime_error("No unit selected!");
        }
    }
    else {
        switch (in_tree) {
        case InTree::ARMY:
            army->take_snapshot_of(current->id());
            current->remove_mount_option(extra, false);
            army->update_on(current->id());
            return true;
        case InTree::ROSTER:
            current->remove_mount_option(extra, false);
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
    if (current == nullptr) throw std::runtime_error("No unit selected!");
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
