#include "normal_unit.h"

normal_unit::normal_unit(std::shared_ptr<base_unit> base)
    : unit(base),
      handle(std::dynamic_pointer_cast<base_normal_unit>(base)) {
    points_ = handle->min_size() * handle->points_per_model();
    model_select_ = ModelSelect::DEFAULT;
    size_ = handle->min_size();
    champ_magic_item_points = 0.0;
    champ_faction_item_points = 0.0;
    champ_total_item_points = 0.0;
    for (const auto& x : handle->eq().weapons) {
        auto p = x.second;
        weapons_[x.first] = {p.first, p.second, 0.0};
    }
    for (const auto& x : handle->eq().armour) {
        auto p = x.second;
        armours_[x.first] = {p.first, p.second, 0.0};
    }
}

normal_unit::normal_unit(const normal_unit& other)
    : unit(other), model_select_(other.model_select_),
      size_(other.size_), weapons_(other.weapons_),
      armours_(other.armours_), oco_extra_(other.oco_extra_),
      mc_extras_(other.mc_extras_), champ_weapons_(other.champ_weapons_),
      champ_armours_(other.champ_armours_), champ_oco_extra_(other.champ_oco_extra_),
      champ_mc_extras_(other.champ_mc_extras_),
      champ_magic_item_points(other.champ_magic_item_points),
      champ_faction_item_points(other.champ_faction_item_points),
      champ_total_item_points(other.champ_total_item_points),
      command_group(other.command_group),
      banner(other.banner), handle(other.handle) {}

// current property accessors

std::size_t normal_unit::size() const noexcept { return size_; }

std::unordered_map<
    WeaponType,
    std::tuple<ItemClass, std::string, double>
> normal_unit::weapons() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return weapons_;
    case ModelSelect::CHAMPION:
        return champ_weapons_;
    }
 }

std::tuple<ItemClass, std::string, double> normal_unit::melee_weapon() const {
    auto search = weapons_.find(WeaponType::MELEE);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::ranged_weapon() const {
    auto search = weapons_.find(WeaponType::BALLISTIC);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::champion_melee_weapon() const {
    auto search = champ_weapons_.find(WeaponType::MELEE);
    if (search != champ_weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::champion_ranged_weapon() const {
    auto search = champ_weapons_.find(WeaponType::BALLISTIC);
    if (search != champ_weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> normal_unit::armour() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return armours_;
    case ModelSelect::CHAMPION:
        return champ_armours_;
    }
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> normal_unit::champion_armour() const noexcept {
    return champ_armours_;
}

std::pair<
    std::string,
    std::pair<bool, double>
> normal_unit::oco_extra() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return oco_extra_;
    case ModelSelect::CHAMPION:
        return {champ_oco_extra_.first, {true, champ_oco_extra_.second}};
    }
 }

std::unordered_map<
    std::string,
    std::pair<bool, double>
> normal_unit::mc_extras() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return mc_extras_;
    case ModelSelect::CHAMPION:
    {
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        > tmp;
        for (const auto& x : champ_mc_extras_)
            tmp[x.first] = {true, x.second};
        return tmp;
    }
    }
 }

std::pair<
    std::string,
    double
> normal_unit::champion_oco_extra() const noexcept { return champ_oco_extra_; }

std::unordered_map<
    std::string,
    double
> normal_unit::champion_mc_extras() const noexcept { return champ_mc_extras_; }

std::pair<
    std::string,
    std::pair<armies::UnitClass, double>
> normal_unit::mount() const noexcept { return mount_; }

std::unordered_map<
    CommandGroup, std::pair<std::string, double>
> normal_unit::command() const noexcept {
    return command_group;
}

std::pair<std::string, double> normal_unit::magic_banner() const noexcept {
    return banner;
}

ModelSelect normal_unit::model_select() const noexcept { return model_select_; }
void normal_unit::switch_model_select(ModelSelect ms) { model_select_ = ms; }

// current property modifiers

void normal_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch(model_select_) {
    case ModelSelect::DEFAULT:
        pick_default_weapon(item_type, name);
        break;
    case ModelSelect::CHAMPION:
        pick_champion_weapon(item_type, name);
        break;
    }
}

void normal_unit::pick_default_weapon(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->opt().opt_weapons.find(name);
        if (search == handle->opt().opt_weapons.cend()) {
            throw std::invalid_argument("Weapon not found!");
        }
        remove_weapon(std::get<0>(search->second));
        weapons_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += size_ * std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        throw std::invalid_argument("Cannot give magic weapons to this unit!");
    case ItemClass::FACTION:
        throw std::invalid_argument("Cannot give faction weapons to this unit!");
    }
}

void normal_unit::pick_champion_weapon(ItemClass item_type, std::string name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->champion_opt().opt_weapons.find(name);
        if (search == handle->champion_opt().opt_weapons.cend())
            throw std::invalid_argument("Weapon not found!");
        remove_champion_weapon(std::get<0>(search->second));
        champ_weapons_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    {
        auto search = handle->magic_items_handle()->find(name);
        if (search == handle->magic_items_handle()->end())
            throw std::invalid_argument("Weapon not found!");
        double mi_budget = handle->champion_magic_item_budget();
        double ti_budget = handle->champion_total_item_budget();
        if ((search->second.points + champ_magic_item_points > mi_budget) ||
                (search->second.points + champ_total_item_points > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the magic weapon has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic weapons' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Unit champion cannot take this weapon!");
        }
        remove_champion_weapon(search->second.weapon_type);
        champ_weapons_[search->second.weapon_type] = {
            item_type,
            search->first,
            search->second.points
        };
        points_ += search->second.points;
        champ_magic_item_points += search->second.points;
        champ_faction_item_points += search->second.points;
        break;
    }
    case ItemClass::FACTION:
    {
        // TODO: implement once we have a faction_items structure
        // => implementation should be very similar to ItemClass::MAGIC case
        break;
    }
    case ItemClass::COMMON:
    {
        // TODO: implement once we have a common_items structure
        // => implementation should be very similar to ItemClass::MAGIC case
        break;
    }
    }
}

void normal_unit::pick_armour(ItemClass item_type, std::string name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        pick_default_armour(item_type, name);
        break;
    case ModelSelect::CHAMPION:
        pick_champion_armour(item_type, name);
        break;
    }
}

void normal_unit::pick_default_armour(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->opt().opt_armour.find(name);
        if (search == handle->opt().opt_armour.end())
            throw std::invalid_argument("Armour not found!");
        remove_armour(std::get<0>(search->second));
        armours_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += size_ * std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        throw std::invalid_argument("Cannot give magic armour to this unit!");
    case ItemClass::FACTION:
        throw std::invalid_argument("Cannot give faction armour to this unit!");
    }
}

void normal_unit::pick_champion_armour(ItemClass item_type, std::string name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->champion_opt().opt_armour.find(name);
        if (search == handle->champion_opt().opt_armour.end())
            throw std::invalid_argument("Armour not found!");
        remove_champion_armour(std::get<0>(search->second));
        champ_armours_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    {
        auto search = handle->magic_items_handle()->find(name);
        if (search == handle->magic_items_handle()->end())
            throw std::invalid_argument("Armour not found!");
        double mi_budget = handle->champion_magic_item_budget();
        double ti_budget = handle->champion_total_item_budget();
        if ((search->second.points + champ_magic_item_points > mi_budget) ||
                (search->second.points + champ_total_item_points > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the magic armour has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic armours' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Unit champion cannot take this weapon!");
        }
        remove_champion_armour(search->second.armour_type);
        champ_armours_[search->second.armour_type] = {
            item_type,
            search->first,
            search->second.points
        };
        points_ += search->second.points;
        champ_magic_item_points += search->second.points;
        champ_faction_item_points += search->second.points;
        break;
    }
    case ItemClass::FACTION:
    {
        // TODO: implement once we have a faction_items structure
        // => implementation should be very similar to ItemClass::MAGIC case
        break;
    }
    case ItemClass::COMMON:
    {
        // TODO: implement once we have a common_items structure
        // => implementation should be very similar to ItemClass::MAGIC case
        break;
    }
    }
}

void normal_unit::remove_weapon(WeaponType wt) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        remove_default_weapon(wt);
        break;
    case ModelSelect::CHAMPION:
        remove_champion_weapon(wt);
        break;
    }
}

void normal_unit::remove_default_weapon(WeaponType wt) {
    if (!weapons_.count(wt)) return;
    auto weapon = weapons_[wt];
    auto search = handle->eq().weapons.find(wt);
    if (search != handle->eq().weapons.cend()) { // avoid removing default weapon
        if (search->second.second == std::get<1>(weapon)) return;
        weapons_[wt] = {search->second.first, search->second.second, 0.0};
    }
    else weapons_.erase(wt);
    points_ -= size_ * std::get<2>(weapon);
}

void normal_unit::remove_champion_weapon(WeaponType wt) {
    auto weapon = champ_weapons_[wt];
    auto def_w = handle->champion_eq().weapons.at(wt);
    if (def_w.second != std::get<1>(weapon)) {
        const double pts = std::get<2>(weapon);
        switch (std::get<0>(weapon)) {
        case ItemClass::MUNDANE:
            break;
        case ItemClass::MAGIC:
        case ItemClass::COMMON:
            champ_magic_item_points -= pts;
            champ_total_item_points -= pts;
            break;
        case ItemClass::FACTION:
            champ_faction_item_points -= pts;
            champ_total_item_points -= pts;
            break;
        }
        points_ -= pts;
        champ_weapons_[wt] = {def_w.first, def_w.second, 0.0};
    }
}

void normal_unit::remove_armour(ArmourType at) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        remove_default_armour(at);
        break;
    case ModelSelect::CHAMPION:
        remove_champion_armour(at);
        break;
    }
}

void normal_unit::remove_default_armour(ArmourType at) {
    if (!armours_.count(at)) return;
    auto armour = armours_[at];
    auto search = handle->eq().armour.find(at);
    if (search != handle->eq().armour.cend()) { // avoid removing default armour
        if (search->second.second == std::get<1>(armour)) return;
        armours_[at] = {search->second.first, search->second.second, 0.0};
    }
    else armours_.erase(at);
    points_ -= size_ * std::get<2>(armour);
}

void normal_unit::remove_champion_armour(ArmourType at) {
    auto armour = champ_armours_[at];
    auto def_a = handle->champion_eq().armour.at(at);
    if (def_a.second != std::get<1>(armour)) {
        const double pts = std::get<2>(armour);
        switch (std::get<0>(armour)) {
        case ItemClass::MUNDANE:
            break;
        case ItemClass::MAGIC:
        case ItemClass::COMMON:
            champ_magic_item_points -= pts;
            champ_total_item_points -= pts;
            break;
        case ItemClass::FACTION:
            champ_faction_item_points -= pts;
            champ_total_item_points -= pts;
            break;
        }
        points_ -= pts;
        champ_armours_[at] = {def_a.first, def_a.second, 0.0};
    }
}

void normal_unit::add_command_member(CommandGroup member) {
    auto search = handle->optional_command().find(member);
    if (search == handle->optional_command().end())
        throw std::invalid_argument("Command group member not available!");
    command_group[member] = {search->second.first, search->second.second};
    points_ += search->second.second;
}

void normal_unit::remove_command_member(CommandGroup member) {
    auto it = command_group.find(member);
    if (it != command_group.end()) {
        points_ -= it->second.second;
        command_group.erase(member);
    }
}

void normal_unit::pick_banner(ItemClass item_type, std::string name) {
    if (!(command_group.count(CommandGroup::STANDARD_BEARER)))
        throw std::runtime_error("Unit contains no standard bearer!");
    switch (item_type) {
    case ItemClass::MAGIC:
    {
        auto search = handle->magic_items_handle()->find(name);
        if (search == handle->magic_items_handle()->end())
            throw std::invalid_argument("Magic banner not found!");
        if (search->second.points > handle->magic_banner_budget())
            throw std::invalid_argument("Magic banner beyond budget!");
        // check if the magic weapon has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic weapons' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Unit standard bearer cannot take this banner!");
        }
        remove_banner();
        banner.first = name;
        banner.second = search->second.points;
        points_ += search->second.points;
        break;
    }
    case ItemClass::FACTION:
        // TODO: implement once we have a faction_items handle
        break;
    case ItemClass::COMMON:
        // TODO: implement once we have a common_items handle
        break;
    default: break;
    }
}

void normal_unit::remove_banner() {
    if (banner.first.empty()) return;
    points_ -= banner.second;
    banner.first = "";
    banner.second = 0.0;
}

void normal_unit::pick_oco_extra(std::string name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        pick_default_oco_extra(name);
        break;
    case ModelSelect::CHAMPION:
        pick_champion_oco_extra(name);
        break;
    }
}

void normal_unit::pick_default_oco_extra(std::string name) {
    auto search = handle->opt().oco_extras.find(name);
    if (search == handle->opt().oco_extras.end())
        throw std::invalid_argument("Item not found!");
    if (oco_extra_.first == name) return;
    const double prev_pts = oco_extra_.second.second;
    points_ -= (oco_extra_.second.first) ? prev_pts : size_ * prev_pts;
    oco_extra_.first = name;
    oco_extra_.second = search->second;
    const double new_pts = search->second.second;
    points_ += (search->second.first) ? new_pts : size_ * new_pts;
}

void normal_unit::pick_champion_oco_extra(std::string name) {
    auto search = handle->champion_opt().oco_extras.find(name);
    if (search == handle->champion_opt().oco_extras.end())
        throw std::invalid_argument("Item not found!");
    if (champ_oco_extra_.first == name) return;
    points_ -= champ_oco_extra_.second;
    champ_oco_extra_.first = name;
    champ_oco_extra_.second = search->second.second;
    points_ += search->second.second;
}

void normal_unit::pick_mc_extra(std::string name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        pick_default_mc_extra(name);
        break;
    case ModelSelect::CHAMPION:
        pick_champion_mc_extra(name);
        break;
    }
}

void normal_unit::pick_default_mc_extra(std::string name) {
    auto search = handle->opt().mc_extras.find(name);
    if (search == handle->opt().mc_extras.end())
        throw std::invalid_argument("Item not found!");
    if (mc_extras_.count(name)) return;
    mc_extras_[name] = search->second;
    const double pts = search->second.second;
    points_ += (search->second.first) ? pts : size_ * pts;
}

void normal_unit::pick_champion_mc_extra(std::string name) {
    auto search = handle->champion_opt().mc_extras.find(name);
    if (search == handle->champion_opt().mc_extras.end())
        throw std::invalid_argument("Item not found!");
    if (champ_mc_extras_.count(name)) return;
    champ_mc_extras_[name] = search->second.second;
    points_ += search->second.second;
}

void normal_unit::remove_oco_extra() {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        remove_default_oco_extra();
        break;
    case ModelSelect::CHAMPION:
        remove_champion_oco_extra();
        break;
    }
}

void normal_unit::remove_default_oco_extra() {
    points_ -= (oco_extra_.second.first) ? oco_extra_.second.second : size_ * oco_extra_.second.second;
    oco_extra_.first = "";
    oco_extra_.second.second = 0.0;
}

void normal_unit::remove_champion_oco_extra() {
    points_ -= champ_oco_extra_.second;
    champ_oco_extra_.first = "";
    champ_oco_extra_.second = 0.0;
}

void normal_unit::remove_mc_extra(std::string name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        remove_default_mc_extra(name);
        break;
    case ModelSelect::CHAMPION:
        remove_champion_mc_extra(name);
        break;
    }
}

void normal_unit::remove_default_mc_extra(std::string name) {
    auto search = mc_extras_.find(name);
    if (search != mc_extras_.end()) {
        const double pts = search->second.second;
        points_ -= (search->second.first) ? pts : size_ * pts;
        mc_extras_.erase(name);
    }
}

void normal_unit::remove_champion_mc_extra(std::string name) {
    auto search = champ_mc_extras_.find(name);
    if (search != champ_mc_extras_.end()) {
        points_ -= search->second;
        champ_mc_extras_.erase(name);
    }
}

void normal_unit::pick_mount(std::string name) {
    auto search = handle->opt().opt_mounts.find(name);
    if (search == handle->opt().opt_mounts.end())
        throw std::invalid_argument("Mount not found!");
    if (mount_.first == name) return;
    points_ -= size_ * mount_.second.second;
    mount_.first = name;
    mount_.second = search->second;
    points_ += size_ * search->second.second;
}

void normal_unit::remove_mount() {
    points_ -= size_ * mount_.second.second;
    mount_.first = "";
    mount_.second.first = handle->unit_class();
    mount_.second.second = 0.0;
}

void normal_unit::change_size(std::size_t n) {
    if (n < handle->min_size() || n > handle->max_size())
        throw std::invalid_argument("Invalid unit size!");
    points_ = n * handle->points_per_model();
    for (const auto& w : weapons_) points_ += n * std::get<2>(w.second);
    for (const auto& a : armours_) points_ += n * std::get<2>(a.second);
    points_ += (oco_extra_.second.first) ? oco_extra_.second.second : n * oco_extra_.second.second;
    for (const auto& e : mc_extras_)
        points_ += (e.second.first) ? e.second.second : n * e.second.second;
    for (const auto& cw : champ_weapons_) points_ += std::get<2>(cw.second);
    for (const auto& ca : champ_armours_) points_ += std::get<2>(ca.second);
    points_ += champ_oco_extra_.second;
    for (const auto& ce : champ_mc_extras_) points_ += ce.second;
    points_ += n * mount_.second.second;
    for (const auto& m : command_group) points_ += m.second.second;
    points_ += banner.second;
    size_ = n;
}
