#include "mixed_unit.h"

mixed_unit::mixed_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : unit(base, army_handle),
      master_size_(0U), slave_size_(0U),
      handle(std::dynamic_pointer_cast<base_mixed_unit>(base)) {
    model_select_ = ModelSelect::DEFAULT;
    mixed_select_ = MixedSelect::MASTER;
    points_ = handle->master_min_size() * handle->master_points_per_model()
            + handle->slave_min_size() * handle->slave_points_per_model();
    master_size_ = handle->master_min_size();
    slave_size_ = handle->slave_min_size();
    // TODO: population weapons and armour
}

mixed_unit::mixed_unit(const mixed_unit& other)
    : unit(other),
      master_size_(other.master_size_), slave_size_(other.slave_size_),
      mount_(other.mount_), master_weapons_(other.master_weapons_),
      master_armours_(other.master_armours_), master_oco_extra_(other.master_oco_extra_),
      master_mc_extras_(other.master_mc_extras_), master_champ_weapons_(other.master_champ_weapons_),
      master_champ_armours_(other.master_champ_armours_), master_champ_talisman_(other.master_champ_talisman_),
      master_champ_enchanted_item_(other.master_champ_enchanted_item_),
      master_champ_arcane_(other.master_champ_arcane_),
      master_champ_item_extras_(other.master_champ_item_extras_),
      master_champ_oco_extra_(other.master_champ_oco_extra_),
      master_champ_mc_extras_(other.master_champ_mc_extras_),
      master_command_group(other.master_command_group),
      banner(other.banner), slave_weapons_(other.slave_weapons_),
      slave_armours_(other.slave_armours_), slave_oco_extra_(other.slave_oco_extra_),
      slave_mc_extras_(other.slave_mc_extras_), slave_champion_weapons_(other.slave_champion_weapons_),
      slave_champion_armours_(other.slave_champion_armours_),
      slave_champion_oco_extra_(other.slave_champion_oco_extra_),
      slave_champion_mc_extras_(other.slave_champion_mc_extras_),
      slave_command_group(other.slave_command_group), handle(other.handle) {}

bool mixed_unit::is_character() const noexcept { return false; }
bool mixed_unit::is_mage() const noexcept { return false; }
bool mixed_unit::is_mixed() const noexcept { return true; }

bool mixed_unit::switch_model_select(ModelSelect ms) {
    model_select_ = ms;
    return true;
}
bool mixed_unit::switch_mixed_select(MixedSelect ms) {
    mixed_select_ = ms;
    return true;
}

std::size_t mixed_unit::master_size() const noexcept{ return master_size_; }
std::size_t mixed_unit::slave_size() const noexcept{ return slave_size_; }

const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& mixed_unit::master_command() const noexcept {
    return master_command_group;
}
const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& mixed_unit::slave_command() const noexcept {
    return slave_command_group;
}

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& mixed_unit::weapons() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_weapons_;
        case ModelSelect::CHAMPION:
            return master_champ_weapons_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_weapons_;
        case ModelSelect::CHAMPION:
            return slave_champion_weapons_;
        }
        break;
    }
    return master_weapons_;
}
const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& mixed_unit::armour() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_armours_;
        case ModelSelect::CHAMPION:
            return master_champ_armours_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_armours_;
        case ModelSelect::CHAMPION:
            return slave_champion_armours_;
        }
        break;
    }
    return master_armours_;
}
const std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::talisman() const noexcept {
    return master_champ_talisman_;
}
const std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::enchanted_item() const noexcept {
    return master_champ_enchanted_item_;
}
const std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::arcane_item() const noexcept {
    return master_champ_arcane_;
}
const std::unordered_map<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::item_extras() const noexcept {
    return master_champ_item_extras_;
}
const std::pair<
    std::string,
    std::pair<bool, double>
>& mixed_unit::oco_extra() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_oco_extra_;
        case ModelSelect::CHAMPION:
            return master_champ_oco_extra_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_oco_extra_;
        case ModelSelect::CHAMPION:
            return slave_champion_oco_extra_;
        }
        break;
    }
    return master_oco_extra_;
}
const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& mixed_unit::mc_extras() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_mc_extras_;
        case ModelSelect::CHAMPION:
            return master_champ_mc_extras_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_mc_extras_;
        case ModelSelect::CHAMPION:
            return slave_champion_mc_extras_;
        }
        break;
    }
    return master_mc_extras_;
}
const std::tuple<
    std::string,
    double,
    std::pair<std::string, double>,
    std::unordered_map<std::string, double>
>& mixed_unit::mnt() const noexcept {
    return mount_;
}

const std::pair<std::string, std::pair<ItemCategory, double>>& mixed_unit::magic_banner() const noexcept {
    return banner;
}

std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& mixed_unit::weapons_access() noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_weapons_;
        case ModelSelect::CHAMPION:
            return master_champ_weapons_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_weapons_;
        case ModelSelect::CHAMPION:
            return slave_champion_weapons_;
        }
        break;
    }
    return master_weapons_;
}
std::unordered_map<
        ArmourType,
        std::tuple<ItemCategory, std::string, double>
>& mixed_unit::armour_access() noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return master_armours_;
        case ModelSelect::CHAMPION:
            return master_champ_armours_;
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return slave_armours_;
        case ModelSelect::CHAMPION:
            return slave_champion_armours_;
        }
        break;
    }
    return master_armours_;
}
std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::talisman_access() noexcept {
    return master_champ_talisman_;
}
std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::enchanted_item_access() noexcept {
    return master_champ_enchanted_item_;
}
std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::arcane_item_access() noexcept {
    return master_champ_arcane_;
}
std::unordered_map<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::item_extras_access() noexcept {
    return master_champ_item_extras_;
}
std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& mixed_unit::magic_banner_acces() noexcept {
    return banner;
}

// item selectors:

// => weapon selection and removal
std::string mixed_unit::pick_weapon(ItemCategory item_category, const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_master_default_weapon(item_category, name);
        case ModelSelect::CHAMPION:
            return pick_master_champion_weapon(item_category, name);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_slave_default_weapon(item_category, name);
        case ModelSelect::CHAMPION:
            return pick_slave_champion_weapon(item_category, name);
        }
        break;
    }
    return pick_master_default_weapon(item_category, name);
}
std::string mixed_unit::remove_weapon(WeaponType wt, bool replacing) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_master_default_weapon(wt, replacing);
        case ModelSelect::CHAMPION:
            return remove_master_champion_weapon(wt, replacing);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_slave_default_weapon(wt, replacing);
        case ModelSelect::CHAMPION:
            return remove_slave_champion_weapon(wt, replacing);
        }
        break;
    }
    return remove_master_default_weapon(wt, replacing);
}

// ==> MASTER
std::string mixed_unit::pick_master_default_weapon(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->master_opt().weapons().find(name);
        if (search == std::end(handle->master_opt().weapons()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_master_default_weapon(search->second.type);
        do_replacements(search->second.replacements);
        master_weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += master_size_ * search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_master_default_weapon(WeaponType wt, bool replacing) {
    if (!master_weapons_.count(wt)) return std::string();
    auto weapon = master_weapons_[wt];
    auto search_eq = handle->master_eq().weapons().find(wt);
    if (search_eq != std::end(handle->master_eq().weapons())) { // equipment contains weapon of type wt
        if (replacing) { // want to replace weapon of type wt anyway so just erase it
            master_weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                master_weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else { // not replacing so set new weapon of type wt to the one held by equipment
            // don't replace default equipment
            if (search_eq->second.second == std::get<1>(weapon)) return std::string();
            master_weapons_[wt] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else master_weapons_.erase(wt);
    points_ -= master_size_ * std::get<2>(weapon);
    return std::get<1>(weapon);
}
std::string mixed_unit::pick_master_champion_weapon(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->master_champion_opt().weapons().find(name);
        if (search == std::end(handle->master_champion_opt().weapons()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_master_champion_weapon(search->second.type);
        do_replacements(search->second.replacements, true);
        master_champ_weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::FACTION:
    case ItemCategory::COMMON:
        switch_model_select(ModelSelect::CHAMPION);
        try { removed = pick_magic_item(ItemType::WEAPON, item_category, name); }
        catch (const std::exception&) {
            switch_model_select(ModelSelect::DEFAULT);
            throw;
        }
        switch_model_select(ModelSelect::DEFAULT);
        break;
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_master_champion_weapon(WeaponType wt, bool replacing) {
    if (!master_champ_weapons_.count(wt)) return std::string();
    auto weapon = master_champ_weapons_[wt];
    auto search_eq = handle->master_champion_eq().weapons().find(wt);
    if (search_eq != std::end(handle->master_champion_eq().weapons())) {
        if (replacing) {
            master_champ_weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                master_champ_weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else {
            if (search_eq->second.second == std::get<1>(weapon)) return std::string();
            master_champ_weapons_[wt] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else master_champ_weapons_.erase(wt);
    const double pts = std::get<2>(weapon);
    switch (std::get<0>(weapon)) {
    case ItemCategory::MUNDANE:
    case ItemCategory::NONE:
        break;
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    }
    points_ -= pts;
    return std::get<1>(weapon);
}

// ==> SLAVE
std::string mixed_unit::pick_slave_default_weapon(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->slave_opt().weapons().find(name);
        if (search == std::end(handle->slave_opt().weapons()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_slave_default_weapon(search->second.type);
        do_replacements(search->second.replacements);
        slave_weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += slave_size_ * search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_slave_default_weapon(WeaponType wt, bool replacing) {
    if (!slave_weapons_.count(wt)) return std::string();
    auto weapon = slave_weapons_[wt];
    auto search_eq = handle->slave_eq().weapons().find(wt);
    if (search_eq != std::end(handle->slave_eq().weapons())) { // equipment contains weapon of type wt
        if (replacing) { // want to replace weapon of type wt anyway so just erase it
            slave_weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                slave_weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else { // not replacing so set new weapon of type wt to the one held by equipment
            // don't replace default equipment
            if (search_eq->second.second == std::get<1>(weapon)) return std::string();
            slave_weapons_[wt] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else slave_weapons_.erase(wt);
    points_ -= slave_size_ * std::get<2>(weapon);
    return std::get<1>(weapon);
}
std::string mixed_unit::pick_slave_champion_weapon(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->slave_champion_opt().weapons().find(name);
        if (search == std::end(handle->slave_champion_opt().weapons()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_slave_champion_weapon(search->second.type);
        do_replacements(search->second.replacements, true);
        slave_champion_weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_slave_champion_weapon(WeaponType wt, bool replacing) {
    if (!slave_champion_weapons_.count(wt)) return std::string();
    auto weapon = slave_champion_weapons_[wt];
    auto search_eq = handle->slave_champion_eq().weapons().find(wt);
    if (search_eq != std::end(handle->slave_champion_eq().weapons())) {
        if (replacing) {
            slave_champion_weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                slave_champion_weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else {
            if (search_eq->second.second == std::get<1>(weapon)) return std::string();
            slave_champion_weapons_[wt] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else slave_champion_weapons_.erase(wt);
    const double pts = std::get<2>(weapon);
    points_ -= pts;
    return std::get<1>(weapon);
}

// => armour selection and removal
std::string mixed_unit::pick_armour(ItemCategory item_category, const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_master_default_armour(item_category, name);
        case ModelSelect::CHAMPION:
            return pick_master_champion_armour(item_category, name);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_slave_default_armour(item_category, name);
        case ModelSelect::CHAMPION:
            return pick_slave_champion_armour(item_category, name);
        }
        break;
    }
    return pick_master_default_armour(item_category, name);
}
std::string mixed_unit::remove_armour(ArmourType at, bool replacing) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_master_default_armour(at, replacing);
        case ModelSelect::CHAMPION:
            return remove_master_champion_armour(at, replacing);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_slave_default_armour(at, replacing);
        case ModelSelect::CHAMPION:
            return remove_slave_champion_armour(at, replacing);
        }
        break;
    }
    return remove_master_default_armour(at, replacing);
}

// ==> MASTER
std::string mixed_unit::pick_master_default_armour(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->master_opt().armour().find(name);
        if (search == std::end(handle->master_opt().armour()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_master_default_armour(search->second.type);
        do_replacements(search->second.replacements);
        master_armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += master_size_ * search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_master_default_armour(ArmourType at, bool replacing) {
    if (!master_armours_.count(at)) return std::string();
    auto armour = master_armours_[at];
    auto search_eq = handle->master_eq().armours().find(at);
    if (search_eq != std::end(handle->master_eq().armours())) { // equipment contains weapon of type wt
        if (replacing) { // want to replace weapon of type wt anyway so just erase it
            master_armours_.erase(at);
        } else { // not replacing so set new weapon of type wt to the one held by equipment
            // don't replace default equipment
            if (search_eq->second.second == std::get<1>(armour)) return std::string();
            master_armours_[at] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else master_armours_.erase(at);
    points_ -= master_size_ * std::get<2>(armour);
    return std::get<1>(armour);
}
std::string mixed_unit::pick_master_champion_armour(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->master_champion_opt().armour().find(name);
        if (search == std::end(handle->master_champion_opt().armour()))
            throw std::invalid_argument("armour not found!");
        removed = remove_master_champion_armour(search->second.type);
        do_replacements(search->second.replacements, true);
        master_champ_armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::FACTION:
    case ItemCategory::COMMON:
        switch_model_select(ModelSelect::CHAMPION);
        try { removed = pick_magic_item(ItemType::ARMOUR, item_category, name); }
        catch (const std::exception&) {
            switch_model_select(ModelSelect::DEFAULT);
            throw;
        }
        switch_model_select(ModelSelect::DEFAULT);
        break;
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_master_champion_armour(ArmourType at, bool replacing) {
    if (!master_champ_armours_.count(at)) return std::string();
    auto armour = master_champ_armours_[at];
    auto search_eq = handle->master_champion_eq().armours().find(at);
    if (search_eq != std::end(handle->master_champion_eq().armours())) {
        if (replacing) {
            master_champ_armours_.erase(at);
        } else {
            if (search_eq->second.second == std::get<1>(armour)) return std::string();
            master_champ_armours_[at] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else master_champ_armours_.erase(at);
    const double pts = std::get<2>(armour);
    switch (std::get<0>(armour)) {
    case ItemCategory::MUNDANE:
    case ItemCategory::NONE:
        break;
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    }
    points_ -= pts;
    return std::get<1>(armour);
}

// ==> SLAVE
std::string mixed_unit::pick_slave_default_armour(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->slave_opt().armour().find(name);
        if (search == std::end(handle->slave_opt().armour()))
            throw std::invalid_argument("Weapon not found!");
        removed = remove_slave_default_armour(search->second.type);
        do_replacements(search->second.replacements);
        slave_armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += slave_size_ * search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_slave_default_armour(ArmourType at, bool replacing) {
    if (!slave_armours_.count(at)) return std::string();
    auto armour = slave_armours_[at];
    auto search_eq = handle->slave_eq().armours().find(at);
    if (search_eq != std::end(handle->slave_eq().armours())) { // equipment contains weapon of type wt
        if (replacing) { // want to replace weapon of type wt anyway so just erase it
            slave_armours_.erase(at);
        } else { // not replacing so set new weapon of type wt to the one held by equipment
            // don't replace default equipment
            if (search_eq->second.second == std::get<1>(armour)) return std::string();
            slave_armours_[at] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else slave_armours_.erase(at);
    points_ -= slave_size_ * std::get<2>(armour);
    return std::get<1>(armour);
}
std::string mixed_unit::pick_slave_champion_armour(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->slave_champion_opt().armour().find(name);
        if (search == std::end(handle->slave_champion_opt().armour()))
            throw std::invalid_argument("armour not found!");
        removed = remove_slave_champion_armour(search->second.type);
        do_replacements(search->second.replacements, true);
        slave_champion_armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}
std::string mixed_unit::remove_slave_champion_armour(ArmourType at, bool replacing) {
    if (!slave_champion_armours_.count(at)) return std::string();
    auto armour = slave_champion_armours_[at];
    auto search_eq = handle->slave_champion_eq().armours().find(at);
    if (search_eq != std::end(handle->slave_champion_eq().armours())) {
        if (replacing) {
            slave_champion_armours_.erase(at);
        } else {
            if (search_eq->second.second == std::get<1>(armour)) return std::string();
            slave_champion_armours_[at] = {search_eq->second.first, search_eq->second.second, 0.0};
        }
    }
    else slave_champion_armours_.erase(at);
    const double pts = std::get<2>(armour);
    points_ -= pts;
    return std::get<1>(armour);
}

// => talisman selection and removal [NOTE: always refers to champion for non-characters]
std::string mixed_unit::pick_talisman(ItemCategory item_category, const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::TALISMAN, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}
std::string mixed_unit::remove_talisman() {
    std::string removed = master_champ_talisman_.first;
    points_ -= master_champ_talisman_.second.second;
    switch (master_champ_talisman_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= master_champ_talisman_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= master_champ_talisman_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= master_champ_talisman_.second.second;
    master_champ_talisman_.first.clear();
    master_champ_talisman_.second.second = 0.0;
    return removed;
}

// => enchanted item selection and removal [NOTE: always refers to champion for non-characters]
std::string mixed_unit::pick_enchanted_item(ItemCategory item_category, const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::ENCHANTED, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}
std::string mixed_unit::remove_enchanted_item() {
    std::string removed = master_champ_enchanted_item_.first;
    points_ -= master_champ_enchanted_item_.second.second;
    switch (master_champ_enchanted_item_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= master_champ_enchanted_item_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= master_champ_enchanted_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= master_champ_enchanted_item_.second.second;
    master_champ_enchanted_item_.first.clear();
    master_champ_enchanted_item_.second.second = 0.0;
    return removed;
}

// => arcane item selection and removal [NOTE: always refers to champion for non-characters]
std::string mixed_unit::pick_arcane_item(ItemCategory item_category, const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::ARCANE, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}
std::string mixed_unit::remove_arcane_item() {
    std::string removed = master_champ_arcane_.first;
    points_ -= master_champ_arcane_.second.second;
    switch (master_champ_arcane_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= master_champ_arcane_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= master_champ_arcane_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= master_champ_arcane_.second.second;
    master_champ_arcane_.first.clear();
    master_champ_arcane_.second.second = 0.0;
    return removed;
}

// => other magic item selection and removal [NOTE: always refers to champion for non-characters]
std::string mixed_unit::pick_magic_extra(ItemCategory item_category, const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::OTHER, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}
std::string mixed_unit::remove_magic_extra(const std::string& name) {
    auto search = master_champ_item_extras_.find(name);
    if (search == std::end(master_champ_item_extras_))
        throw std::invalid_argument("Unit does not have this item!");
    points_ -= search->second.second;
    switch (search->second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= search->second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= search->second.second;
        break;
    default: break;
    }
    total_item_points_ -= search->second.second;
    master_champ_item_extras_.erase(name);
    return name;
}

// => one-choice-only extra selection and removal
std::string mixed_unit::pick_oco_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_master_default_oco_extra(name);
        case ModelSelect::CHAMPION:
            return pick_master_champion_oco_extra(name);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_slave_default_oco_extra(name);
        case ModelSelect::CHAMPION:
            return pick_slave_champion_oco_extra(name);
        }
        break;
    }
    return pick_master_default_oco_extra(name);
}
std::string mixed_unit::remove_oco_extra() {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_master_default_oco_extra();
        case ModelSelect::CHAMPION:
            return remove_master_champion_oco_extra();
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_slave_default_oco_extra();
        case ModelSelect::CHAMPION:
            return remove_slave_champion_oco_extra();
        }
        break;
    }
    return remove_master_default_oco_extra();
}

// ==> MASTER
std::string mixed_unit::pick_master_default_oco_extra(const std::string& name) {
    auto search = handle->master_opt().oco_extras().find(name);
    if (search == std::end(handle->master_opt().oco_extras()))
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (master_oco_extra_.first == name) return std::string();
    removed = master_oco_extra_.first;
    const double prev_pts = master_oco_extra_.second.second;
    points_ -= (master_oco_extra_.second.first) ? prev_pts : master_size_ * prev_pts;
    master_oco_extra_.first = name;
    master_oco_extra_.second = {search->second.is_singular, search->second.points};
    const double new_pts = search->second.points;
    points_ += (search->second.is_singular) ? new_pts : master_size_ * new_pts;
    return removed;
}
std::string mixed_unit::remove_master_default_oco_extra() {
    std::string removed = master_oco_extra_.first;
    points_ -= (master_oco_extra_.second.first) ? master_oco_extra_.second.second
                                                : master_size_ * master_oco_extra_.second.second;
    master_oco_extra_.first.clear();
    master_oco_extra_.second.second = 0.0;
    return removed;
}
std::string mixed_unit::pick_master_champion_oco_extra(const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->master_champion_opt().oco_extras().find(name);
    if (search == std::end(handle->master_champion_opt().oco_extras()))
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (master_champ_oco_extra_.first == name) return std::string();
    removed = master_champ_oco_extra_.first;
    points_ -= master_champ_oco_extra_.second.second;
    master_champ_oco_extra_.first = name;
    master_champ_oco_extra_.second = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return removed;
}
std::string mixed_unit::remove_master_champion_oco_extra() {
    std::string removed = master_champ_oco_extra_.first;
    points_ -= master_champ_oco_extra_.second.second;
    master_champ_oco_extra_.first = "";
    master_champ_oco_extra_.second = {true, 0.0};
    return removed;
}

// ==> SLAVE
std::string mixed_unit::pick_slave_default_oco_extra(const std::string& name) {
    auto search = handle->slave_opt().oco_extras().find(name);
    if (search == std::end(handle->slave_opt().oco_extras()))
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (slave_oco_extra_.first == name) return std::string();
    removed = slave_oco_extra_.first;
    const double prev_pts = slave_oco_extra_.second.second;
    points_ -= (slave_oco_extra_.second.first) ? prev_pts : slave_size_ * prev_pts;
    slave_oco_extra_.first = name;
    slave_oco_extra_.second = {search->second.is_singular, search->second.points};
    const double new_pts = search->second.points;
    points_ += (search->second.is_singular) ? new_pts : slave_size_ * new_pts;
    return removed;
}
std::string mixed_unit::remove_slave_default_oco_extra() {
    std::string removed = slave_oco_extra_.first;
    points_ -= (slave_oco_extra_.second.first) ? slave_oco_extra_.second.second
                                                : slave_size_ * slave_oco_extra_.second.second;
    slave_oco_extra_.first.clear();
    slave_oco_extra_.second.second = 0.0;
    return removed;
}
std::string mixed_unit::pick_slave_champion_oco_extra(const std::string& name) {
    if (!(slave_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->slave_champion_opt().oco_extras().find(name);
    if (search == std::end(handle->slave_champion_opt().oco_extras()))
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (slave_champion_oco_extra_.first == name) return std::string();
    removed = slave_champion_oco_extra_.first;
    points_ -= slave_champion_oco_extra_.second.second;
    slave_champion_oco_extra_.first = name;
    slave_champion_oco_extra_.second = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return removed;
}
std::string mixed_unit::remove_slave_champion_oco_extra() {
    std::string removed = slave_champion_oco_extra_.first;
    points_ -= slave_champion_oco_extra_.second.second;
    slave_champion_oco_extra_.first = "";
    slave_champion_oco_extra_.second = {true, 0.0};
    return removed;
}

// => multiple-choice extra selection and removal
std::string mixed_unit::pick_mc_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_master_default_mc_extra(name);
        case ModelSelect::CHAMPION:
            return pick_master_champion_mc_extra(name);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return pick_slave_default_mc_extra(name);
        case ModelSelect::CHAMPION:
            return pick_slave_champion_mc_extra(name);
        }
        break;
    }
    return pick_master_default_mc_extra(name);
}
std::string mixed_unit::remove_mc_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_master_default_mc_extra(name);
        case ModelSelect::CHAMPION:
            return remove_master_champion_mc_extra(name);
        }
        break;
    case MixedSelect::SLAVE:
        switch (model_select_) {
        case ModelSelect::DEFAULT:
            return remove_slave_default_mc_extra(name);
        case ModelSelect::CHAMPION:
            return remove_slave_champion_mc_extra(name);
        }
        break;
    }
    return remove_master_default_mc_extra(name);
}

// ==> MASTER
std::string mixed_unit::pick_master_default_mc_extra(const std::string& name) {
    auto search = handle->master_opt().mc_extras().find(name);
    if (search == std::end(handle->master_opt().mc_extras()))
        throw std::invalid_argument("Item not found!");
    auto restr = restriction_check(search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    if (master_mc_extras_.count(name)) return std::string();
    master_mc_extras_[name] = {search->second.is_singular, search->second.points};
    const double pts = search->second.points;
    points_ += (search->second.is_singular) ? pts : master_size_ * pts;
    return std::string();
}
std::string mixed_unit::remove_master_default_mc_extra(const std::string& name) {
    auto search = master_mc_extras_.find(name);
    if (search != std::end(master_mc_extras_)) {
        const double pts = search->second.second;
        points_ -= (search->second.first) ? pts : master_size_ * pts;
        master_mc_extras_.erase(name);
    }
    return std::string();
}
std::string mixed_unit::pick_master_champion_mc_extra(const std::string& name) {
    if (!(master_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->master_champion_opt().mc_extras().find(name);
    if (search == std::end(handle->master_champion_opt().mc_extras()))
        throw std::invalid_argument("Item not found!");
    if (master_champ_mc_extras_.count(name)) return std::string();
    master_champ_mc_extras_[name] = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return std::string();
}
std::string mixed_unit::remove_master_champion_mc_extra(const std::string& name) {
    auto search = master_champ_mc_extras_.find(name);
    if (search != std::end(master_champ_mc_extras_)) {
        points_ -= search->second.second;
        master_champ_mc_extras_.erase(name);
    }
    return std::string();
}

// ==> SLAVE
std::string mixed_unit::pick_slave_default_mc_extra(const std::string& name) {
    auto search = handle->slave_opt().mc_extras().find(name);
    if (search == std::end(handle->slave_opt().mc_extras()))
        throw std::invalid_argument("Item not found!");
    auto restr = restriction_check(search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    if (slave_mc_extras_.count(name)) return std::string();
    slave_mc_extras_[name] = {search->second.is_singular, search->second.points};
    const double pts = search->second.points;
    points_ += (search->second.is_singular) ? pts : slave_size_ * pts;
    return std::string();
}
std::string mixed_unit::remove_slave_default_mc_extra(const std::string& name) {
    auto search = slave_mc_extras_.find(name);
    if (search != std::end(slave_mc_extras_)) {
        const double pts = search->second.second;
        points_ -= (search->second.first) ? pts : slave_size_ * pts;
        slave_mc_extras_.erase(name);
    }
    return std::string();
}
std::string mixed_unit::pick_slave_champion_mc_extra(const std::string& name) {
    if (!(slave_command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->slave_champion_opt().mc_extras().find(name);
    if (search == std::end(handle->slave_champion_opt().mc_extras()))
        throw std::invalid_argument("Item not found!");
    if (slave_champion_mc_extras_.count(name)) return std::string();
    slave_champion_mc_extras_[name] = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return std::string();
}
std::string mixed_unit::remove_slave_champion_mc_extra(const std::string& name) {
    auto search = slave_champion_mc_extras_.find(name);
    if (search != std::end(slave_champion_mc_extras_)) {
        points_ -= search->second.second;
        slave_champion_mc_extras_.erase(name);
    }
    return std::string();
}

// => mount selection and removal
void mixed_unit::pick_mount(const std::string& name) {
    (void)name;
}
void mixed_unit::remove_mount() {

}

// => banner selection and removal
std::string mixed_unit::pick_banner(ItemCategory item_category, const std::string& name) {
    if (!(master_command_group.count(CommandGroup::STANDARD_BEARER)))
        throw std::runtime_error("Unit contains no standard bearer!");
    std::string removed;
    try { removed = pick_magic_item(ItemType::BANNER, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}
std::string mixed_unit::remove_banner() {
    std::string removed = banner.first;
    if (banner.first.empty()) return std::string();
    points_ -= banner.second.second;
    banner.first.clear();
    banner.second.second = 0.0;
    return removed;
}

// => command group selection and removal
void mixed_unit::add_command_member(CommandGroup member){
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        add_master_command_member(member);
        break;
    case MixedSelect::SLAVE:
        add_slave_command_member(member);
        break;
    }
}
void mixed_unit::remove_command_member(CommandGroup member){
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        remove_master_command_member(member);
        break;
    case MixedSelect::SLAVE:
        remove_slave_command_member(member);
        break;
    }
}

void mixed_unit::add_master_command_member(CommandGroup member) {
    auto search = handle->master_optional_command().find(member);
    if (search == std::end(handle->master_optional_command()))
        throw std::invalid_argument("Command group member not available!");
    master_command_group[member] = {search->second.first, search->second.second};
    points_ += search->second.second;
}
void mixed_unit::remove_master_command_member(CommandGroup member) {
    auto search = master_command_group.find(member);
    if (search != std::end(master_command_group)) {
        points_ -= search->second.second;
        if (search->first == CommandGroup::CHAMPION) {
            for (const auto& x : master_champ_weapons_)
                remove_master_champion_weapon(x.first);
            for (const auto& x : master_champ_armours_)
                remove_master_champion_armour(x.first);
            remove_master_champion_oco_extra();
            std::vector<std::string> mc_extra_names;
            for (const auto& extra : master_champ_mc_extras_) mc_extra_names.push_back(extra.first);
            for (const auto& x : mc_extra_names) remove_master_champion_mc_extra(x);
            remove_talisman();
            remove_enchanted_item();
            remove_arcane_item();
            std::vector<std::string> magic_extra_names;
            for (const auto& extra : master_champ_item_extras_) magic_extra_names.push_back(extra.first);
            for (const auto& x : magic_extra_names) remove_magic_extra(x);
        } else if (search->first == CommandGroup::STANDARD_BEARER)
            remove_banner();
        master_command_group.erase(member);
    }
}
void mixed_unit::add_slave_command_member(CommandGroup member) {
    auto search = handle->slave_optional_command().find(member);
    if (search == std::end(handle->slave_optional_command()))
        throw std::invalid_argument("Command group member not available!");
    slave_command_group[member] = {search->second.first, search->second.second};
    points_ += search->second.second;
}
void mixed_unit::remove_slave_command_member(CommandGroup member) {
    auto search = slave_command_group.find(member);
    if (search != std::end(slave_command_group)) {
        points_ -= search->second.second;
        if (search->first == CommandGroup::CHAMPION) {
            for (const auto& x : slave_champion_weapons_)
                remove_slave_champion_weapon(x.first);
            for (const auto& x : slave_champion_armours_)
                remove_slave_champion_armour(x.first);
            remove_slave_champion_oco_extra();
            std::vector<std::string> mc_extra_names;
            for (const auto& extra : slave_champion_mc_extras_) mc_extra_names.push_back(extra.first);
            for (const auto& x : mc_extra_names) remove_slave_champion_mc_extra(x);
        } else if (search->first == CommandGroup::STANDARD_BEARER)
            remove_banner();
        slave_command_group.erase(member);
    }
}

// changing unit size
void mixed_unit::change_size(std::size_t n){
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        change_master_size(n);
        break;
    case MixedSelect::SLAVE:
        change_slave_size(n);
        break;
    }
}
void mixed_unit::change_master_size(std::size_t n) {
    if (n < handle->master_min_size() || n > handle->master_max_size())
        throw std::invalid_argument("Invalid unit size!");
    if (static_cast<double>(n)/slave_size_ > handle->ratio_limit()) {
        std::string msg = "Cannot take " + std::to_string(n) + ' '
                + handle->master_name() + " models for only "
                + std::to_string(slave_size_) + ' ' + handle->slave_name()
                + " models!";
        throw std::invalid_argument(msg);
    }
    // reset points value to n * master_pts/model + slave_size_ * slave_pts/model
    points_ = n * handle->master_points_per_model() + slave_size_ * handle->slave_points_per_model();
    // MASTER default
    for (const auto& w : master_weapons_) points_ += n * std::get<2>(w.second);
    for (const auto& a : master_armours_) points_ += n * std::get<2>(a.second);
    points_ += (master_oco_extra_.second.first) ? master_oco_extra_.second.second :
                                                  n*master_oco_extra_.second.second;
    for (const auto& e : master_mc_extras_)
        points_ += (e.second.first) ? e.second.second : n * e.second.second;
    // MASTER champion
    for (const auto& w : master_champ_weapons_) points_ += std::get<2>(w.second);
    for (const auto& a : master_champ_armours_) points_ += std::get<2>(a.second);
    points_ += master_champ_oco_extra_.second.second;
    for (const auto& e : master_champ_mc_extras_) points_ += e.second.second;
    points_ += master_champ_talisman_.second.second;
    points_ += master_champ_enchanted_item_.second.second;
    points_ += master_champ_arcane_.second.second;
    for (const auto& ce : master_champ_item_extras_) points_ += ce.second.second;
    // SLAVE default
    for (const auto& w : slave_weapons_) points_ += slave_size_ * std::get<2>(w.second);
    for (const auto& a : slave_armours_) points_ += slave_size_ * std::get<2>(a.second);
    points_ += (slave_oco_extra_.second.first) ? slave_oco_extra_.second.second :
                                                  slave_size_*slave_oco_extra_.second.second;
    for (const auto& e : slave_mc_extras_)
        points_ += (e.second.first) ? e.second.second : slave_size_ * e.second.second;
    // SLAVE champion
    for (const auto& w : slave_champion_weapons_) points_ += std::get<2>(w.second);
    for (const auto& a : slave_champion_armours_) points_ += std::get<2>(a.second);
    points_ += slave_champion_oco_extra_.second.second;
    for (const auto& e : slave_champion_mc_extras_) points_ += e.second.second;
    // add back command group and banner points
    for (const auto& mm : master_command_group) points_ += mm.second.second;
    for (const auto& sm : slave_command_group) points_ += sm.second.second;
    points_ += banner.second.second;
    master_size_ = n;
}
void mixed_unit::change_slave_size(std::size_t n){
    if (master_size_/static_cast<double>(n) > handle->ratio_limit()) {
        std::string msg = "Cannot take only " + std::to_string(n) + ' '
                + handle->slave_name() + " models with "
                + std::to_string(master_size_) + ' ' + handle->master_name()
                + " models!";
        throw std::invalid_argument(msg);
    }
    points_ = master_size_ * handle->master_points_per_model() + n * handle->slave_points_per_model();
    // MASTER default
    for (const auto& w : master_weapons_) points_ += master_size_ * std::get<2>(w.second);
    for (const auto& a : master_armours_) points_ += master_size_ * std::get<2>(a.second);
    points_ += (master_oco_extra_.second.first) ? master_oco_extra_.second.second :
                                                  master_size_*master_oco_extra_.second.second;
    for (const auto& e : master_mc_extras_)
        points_ += (e.second.first) ? e.second.second : master_size_ * e.second.second;
    // MASTER champion
    for (const auto& w : master_champ_weapons_) points_ += std::get<2>(w.second);
    for (const auto& a : master_champ_armours_) points_ += std::get<2>(a.second);
    points_ += master_champ_oco_extra_.second.second;
    for (const auto& e : master_champ_mc_extras_) points_ += e.second.second;
    points_ += master_champ_talisman_.second.second;
    points_ += master_champ_enchanted_item_.second.second;
    points_ += master_champ_arcane_.second.second;
    for (const auto& ce : master_champ_item_extras_) points_ += ce.second.second;
    // SLAVE default
    for (const auto& w : slave_weapons_) points_ += n * std::get<2>(w.second);
    for (const auto& a : slave_armours_) points_ += n * std::get<2>(a.second);
    points_ += (slave_oco_extra_.second.first) ? slave_oco_extra_.second.second :
                                                  n*slave_oco_extra_.second.second;
    for (const auto& e : slave_mc_extras_)
        points_ += (e.second.first) ? e.second.second : n * e.second.second;
    // SLAVE champion
    for (const auto& w : slave_champion_weapons_) points_ += std::get<2>(w.second);
    for (const auto& a : slave_champion_armours_) points_ += std::get<2>(a.second);
    points_ += slave_champion_oco_extra_.second.second;
    for (const auto& e : slave_champion_mc_extras_) points_ += e.second.second;
    // add back command group and banner points
    for (const auto& mm : master_command_group) points_ += mm.second.second;
    for (const auto& sm : slave_command_group) points_ += sm.second.second;
    points_ += banner.second.second;
    slave_size_ = n;
}

// serialisation and exporting
std::string mixed_unit::html_table_row() const {
    return "";
}
std::string mixed_unit::save() const {
    return "";
}

/*std::string mixed_unit::html_table_row() const {
    std::string row = "<tr>";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit size
    row += "<td>" + std::to_string(master_.size()) + " Packmasters<br/>" +
            std::to_string(slave_.size()) + " " + name() + "</td>\n";
    // unit mount
    row += "<td>&nbsp;</td>\n";
    // weapons
    if (slave_.weapons().count(WeaponType::MELEE))
        row += "<td><strong>Melee:</strong> " +
                std::get<1>(slave_.weapons().at(WeaponType::MELEE)) +
                (slave_.weapons().count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (slave_.weapons().count(WeaponType::BALLISTIC))
        row +=  std::string(slave_.weapons().count(WeaponType::MELEE) ? "" : "<td>") + "Ranged: " +
                std::get<1>(slave_.weapons().at(WeaponType::BALLISTIC)) + "</td>\n";
    if (!slave_.weapons().count(WeaponType::MELEE) &&
            !slave_.weapons().count(WeaponType::BALLISTIC)) row += "<td>&nbsp;</td>";
    // armour
    if (slave_.armour().count(ArmourType::ARMOUR))
        row += "<td>Body: " +
                std::get<1>(slave_.armour().at(ArmourType::ARMOUR)) +
                (slave_.armour().count(ArmourType::SHIELD) || slave_.armour().count(ArmourType::HELMET)
                    ? "<br/>" : "</td>\n");
    if (slave_.armour().count(ArmourType::SHIELD))
        row +=  std::string(slave_.armour().count(ArmourType::ARMOUR) ? "" : "<td>") + "Shield: " +
                std::get<1>(slave_.armour().at(ArmourType::SHIELD)) +
                (slave_.armour().count(ArmourType::HELMET) ? "<br/>" : "</td>\n");
    if (slave_.armour().count(ArmourType::HELMET))
        row +=  std::string((slave_.armour().count(ArmourType::ARMOUR)
                             || slave_.armour().count(ArmourType::SHIELD))
                            ? "" : "<td>") + "Helmet: " +
                std::get<1>(slave_.armour().at(ArmourType::HELMET)) + "</td>\n";
    if (!slave_.armour().count(ArmourType::ARMOUR) &&
            !slave_.armour().count(ArmourType::SHIELD) &&
            !slave_.armour().count(ArmourType::HELMET)) row += "<td>&nbsp;</td>";
    // extras
    row += "<td>";
    if (!slave_.oco_extra().first.empty()) row += slave_.oco_extra().first + (slave_.mc_extras().empty() ? "" : "<br/>");
    for (const auto& x : slave_.mc_extras()) row += x.first + "<br/>";
    if (slave_.oco_extra().first.empty() && slave_.mc_extras().empty()) row += "&nbsp;";
    row += "</td>\n";
    // command
    if (slave_.command().count(CommandGroup::MUSICIAN))
        row += "<td>" +
                slave_.command().at(CommandGroup::MUSICIAN).first +
                (slave_.command().count(CommandGroup::STANDARD_BEARER)
                 || slave_.command().count(CommandGroup::CHAMPION)
                    ? "<br/>" : "</td>\n");
    if (slave_.command().count(CommandGroup::STANDARD_BEARER))
        row +=  std::string(slave_.command().count(CommandGroup::MUSICIAN) ? "" : "<td>") +
                slave_.command().at(CommandGroup::STANDARD_BEARER).first +
                (slave_.command().count(CommandGroup::CHAMPION) ? "<br/>" : "</td>\n");
    if (slave_.command().count(CommandGroup::CHAMPION))
        row +=  std::string((slave_.command().count(CommandGroup::MUSICIAN)
                             || slave_.command().count(CommandGroup::STANDARD_BEARER))
                            ? "" : "<td>") + slave_.command().at(CommandGroup::CHAMPION).first + "</td>\n";
    if (!slave_.command().count(CommandGroup::MUSICIAN) &&
            !slave_.command().count(CommandGroup::STANDARD_BEARER) &&
            !slave_.command().count(CommandGroup::CHAMPION)) row += "<td>&nbsp;</td>";
    // banner
    row += "<td>";
    if (!slave_.magic_banner().first.empty()) row += slave_.magic_banner().first;
    else row += "&nbsp;";
    row += "</td>\n";
    // characteristics
    row += "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    row += "<thead><tr>\n"
            "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
            "<th>I</th><th>A</th><th>Ld</th>\n"
           "</tr></thead>\n";
    // => slave characteristics
    row += "<tr>\n";
    for (const auto& x : handle->slave().statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    // => master characteristics
    row += "<tr>\n";
    for (const auto& x : handle->master().statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}*/
