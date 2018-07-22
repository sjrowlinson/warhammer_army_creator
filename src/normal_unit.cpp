#include "normal_unit.h"

normal_unit::normal_unit(std::shared_ptr<_base_unit> base)
    : _unit(base),
      handle(std::dynamic_pointer_cast<base_normal_unit>(base)) {
    points_ = handle->min_size() * handle->points_per_model();
    size_ = handle->min_size();
    champ_magic_item_points = 0.0;
    champ_faction_item_points = 0.0;
    champ_total_item_points = 0.0;
    for (const auto& x : handle->eq().weapons) {
        auto p = x.second;
        weapons[x.first] = {p.first, p.second, 0.0};
    }
    for (const auto& x : handle->eq().armour) {
        auto p = x.second;
        armours[x.first] = {p.first, p.second, 0.0};
    }
}

// current property accessors

std::size_t normal_unit::size() const noexcept { return size_; }
std::tuple<ItemClass, std::string, double> normal_unit::melee_weapon() const {
    auto search = weapons.find(WeaponType::MELEE);
    if (search != weapons.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::ranged_weapon() const {
    auto search = weapons.find(WeaponType::BALLISTIC);
    if (search != weapons.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::champion_melee_weapon() const {
    auto search = champ_weapons.find(WeaponType::MELEE);
    if (search != champ_weapons.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> normal_unit::champion_ranged_weapon() const {
    auto search = champ_weapons.find(WeaponType::BALLISTIC);
    if (search != champ_weapons.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> normal_unit::armour() const noexcept {
    return armours;
}
std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> normal_unit::champion_armour() const noexcept {
    return champ_armours;
}

std::unordered_map<
    std::string,
    std::pair<bool, double>
> normal_unit::extras() const noexcept {
    return extras_;
}

std::unordered_map<
    std::string,
    std::pair<bool, double>
> normal_unit::champion_extras() const noexcept {
    return champ_extras;
}

std::unordered_map<
    CommandGroup, std::pair<std::string, double>
> normal_unit::command() const noexcept {
    return command_group;
}

std::pair<std::string, double> normal_unit::magic_banner() const noexcept {
    return banner;
}

// current property modifiers

void normal_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->opt().opt_weapons.find(name);
        if (search == handle->opt().opt_weapons.end())
            throw std::invalid_argument("Weapon not found!");
        remove_weapon(std::get<0>(search->second));
        weapons[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += size_ * std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    {
        throw std::invalid_argument("Cannot give magic weapons to this unit!");
        break;
    }
    case ItemClass::FACTION:
    {
        throw std::invalid_argument("Cannot give faction weapons to this unit!");
        break;
    }
    default: break;
    }
}

void normal_unit::pick_armour(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->opt().opt_armour.find(name);
        if (search == handle->opt().opt_armour.end())
            throw std::invalid_argument("Armour not found!");
        remove_armour(std::get<0>(search->second));
        armours[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += size_ * std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    {
        throw std::invalid_argument("Cannot give magic armour to this unit!");
        break;
    }
    case ItemClass::FACTION:
    {
        throw std::invalid_argument("Cannot give faction armour to this unit!");
        break;
    }
    default: break;
    }
}

void normal_unit::pick_champion_weapon(ItemClass item_type, std::string name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->champion_opt().opt_weapons.find(name);
        if (search == handle->champion_opt().opt_weapons.end())
            throw std::invalid_argument("Weapon not found!");
        remove_champion_weapon(std::get<0>(search->second));
        champ_weapons[std::get<0>(search->second)] = {
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
        champ_weapons[search->second.weapon_type] = {
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
    default: break;
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
        champ_armours[std::get<0>(search->second)] = {
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
        champ_armours[search->second.armour_type] = {
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
    default: break;
    }
}

void normal_unit::remove_weapon(WeaponType wt) {
    auto weapon = weapons[wt];
    auto def_w = handle->eq().weapons.at(wt);
    // do not want to remove a default weapon
    if (def_w.second != std::get<1>(weapon)) {
        points_ -= size_ * std::get<2>(weapon);
        weapons[wt] = {def_w.first, def_w.second, 0.0};
    }
}

void normal_unit::remove_armour(ArmourType at) {
    auto armour = armours[at];
    auto def_a = handle->eq().armour.at(at);
    // do not want to remove a default armour
    if (def_a.second != std::get<1>(armour)) {
        points_ -= size_ * std::get<2>(armour);
        armours[at] = {def_a.first, def_a.second, 0.0};
    }
}

void normal_unit::remove_champion_weapon(WeaponType wt) {
    auto weapon = weapons[wt];
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
        default: break;
        }
        points_ -= pts;
        weapons[wt] = {def_w.first, def_w.second, 0.0};
    }
}

void normal_unit::remove_champion_armour(ArmourType at) {
    auto armour = armours[at];
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
        default: break;
        }
        points_ -= pts;
        armours[at] = {def_a.first, def_a.second, 0.0};
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

void normal_unit::pick_extra(std::string name) {
    auto search = handle->opt().opt_extras.first.find(name);
    if (search == handle->opt().opt_extras.first.end())
        throw std::invalid_argument("Item not found!");
    if (handle->opt().opt_extras.second) { // one choice only
        for (const auto& x : extras_) {
            if (x.second.first) points_ -= x.second.second;
            else points_ -= size_ * x.second.second;
        }
        extras_.clear();
    }
    extras_[search->first] = search->second;
    if (search->second.first) points_ += search->second.second;
    else points_ += size_ * search->second.second;
}

void normal_unit::pick_champion_extra(std::string name) {
    auto search = handle->champion_opt().opt_extras.first.find(name);
    if (search == handle->champion_opt().opt_extras.first.end())
        throw std::invalid_argument("Item not found!");
    if (handle->champion_opt().opt_extras.second) { // one choice only
        for (const auto& x : champ_extras)
            points_ -= x.second.second;
        champ_extras.clear();
    }
    champ_extras[search->first] = search->second;
    points_ += search->second.second;
}

void normal_unit::remove_extra(std::string name) {
    auto search = extras_.find(name);
    if (search != extras_.end()) {
        if (search->second.first) points_ -= search->second.second;
        else points_ -= size_ * search->second.second;
        extras_.erase(name);
    }
}

void normal_unit::remove_champion_extra(std::string name) {
    auto search = champ_extras.find(name);
    if (search != champ_extras.end()) {
        points_ -= search->second.second;
        champ_extras.erase(name);
    }
}

void normal_unit::change_size(std::size_t n) {
    if (n < handle->min_size() || n > handle->max_size())
        throw std::invalid_argument("Invalid unit size!");
    points_ = n * handle->points_per_model();
    for (const auto& w : weapons) points_ += n * std::get<2>(w.second);
    for (const auto& a : armours) points_ += n * std::get<2>(a.second);
    for (const auto& e : extras_) {
        if (e.second.first) points_ += e.second.second;
        else points_ += n * e.second.second;
    }
    for (const auto& cw : champ_weapons) points_ += std::get<2>(cw.second);
    for (const auto& ca : champ_armours) points_ += std::get<2>(ca.second);
    for (const auto& ce : champ_extras) points_ + ce.second.second;
    for (const auto& m : command_group) points_ += m.second.second;
    points_ += banner.second;
    size_ = n;
}

// base property accessors

const std::vector<short>& normal_unit::statistics() const noexcept {
    return handle->statistics();
}

const std::vector<short>& normal_unit::champion_statistics() const noexcept {
    return handle->champion_statistics();
}

const std::vector<std::string>& normal_unit::special_rules() const noexcept {
    return handle->special_rules();
}

const std::vector<std::string>& normal_unit::champion_special_rules() const noexcept {
    return handle->champion_special_rules();
}

const equipment& normal_unit::eq() const noexcept {
    return handle->eq();
}

const options& normal_unit::opt() const noexcept {
    return handle->opt();
}

const equipment& normal_unit::champion_eq() const noexcept {
    return handle->champion_eq();
}

const options& normal_unit::champion_opt() const noexcept {
    return handle->champion_opt();
}

const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
>& normal_unit::optional_command() const noexcept {
    return handle->optional_command();
}

double normal_unit::champion_magic_item_budget() const noexcept {
    return handle->champion_magic_item_budget();
}

double normal_unit::champion_faction_item_budget() const noexcept {
    return handle->champion_faction_item_budget();
}

double normal_unit::champion_total_item_budget() const noexcept {
    return handle->champion_total_item_budget();
}

double normal_unit::magic_banner_budget() const noexcept {
    return handle->magic_banner_budget();
}
