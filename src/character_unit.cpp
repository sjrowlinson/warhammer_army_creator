#include "character_unit.h"

character_unit::character_unit(std::shared_ptr<base_unit> base)
    : unit(base),
      handle_(std::dynamic_pointer_cast<base_character_unit>(base)) {
    points_ = handle_->points();
    for (const auto& w : handle_->eq().weapons) {
        auto p = w.second;
        weapons_[w.first] = {p.first, p.second, 0.0};
    }
    for (const auto& a : handle_->eq().armour) {
        auto p = a.second;
        armours_[a.first] = {p.first, p.second, 0.0};
    }
    magic_item_points_ = 0.0;
    faction_item_points_ = 0.0;
    total_item_points_ = 0.0;
}

std::size_t character_unit::size() const noexcept { return 1U; }

std::unordered_map<
    WeaponType,
    std::tuple<ItemClass, std::string, double>
> character_unit::weapons() const noexcept { return weapons_; }

std::tuple<ItemClass, std::string, double> character_unit::melee_weapon() const {
    auto search = weapons_.find(WeaponType::MELEE);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "Hand weapon", 0.0};
}

std::tuple<ItemClass, std::string, double> character_unit::ranged_weapon() const {
    auto search = weapons_.find(WeaponType::BALLISTIC);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> character_unit::armour() const noexcept { return armours_; }

std::pair<std::string, double> character_unit::oco_extra() const noexcept { return oco_extra_; }
std::unordered_map<std::string, double> character_unit::mc_extras() const noexcept { return mc_extras_; }

void character_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle_->opt().opt_weapons.find(name);
        if (search == handle_->opt().opt_weapons.cend())
            throw std::invalid_argument("Weapon not found!");
        remove_weapon(std::get<0>(search->second));
        weapons_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    {
        auto search = handle_->magic_items_handle()->find(name);
        if (search == handle_->magic_items_handle()->end())
            throw std::invalid_argument("Weapon not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + magic_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the magic weapon has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic weapons' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this weapon!");
        }
        remove_weapon(search->second.weapon_type);
        weapons_[search->second.weapon_type] = {
            item_type,
            search->first,
            search->second.points
        };
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
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

void character_unit::pick_armour(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle_->opt().opt_armour.find(name);
        if (search == handle_->opt().opt_armour.cend())
            throw std::invalid_argument("Armour not found!");
        remove_armour(std::get<0>(search->second));
        armours_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    {
        auto search = handle_->magic_items_handle()->find(name);
        if (search == handle_->magic_items_handle()->end())
            throw std::invalid_argument("Armour not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the magic armour has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic armours' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this weapon!");
        }
        remove_armour(search->second.armour_type);
        armours_[search->second.armour_type] = {
            item_type,
            search->first,
            search->second.points
        };
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
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

void character_unit::pick_oco_extra(std::string name) {
    auto search = handle_->opt().oco_extras.find(name);
    if (search == handle_->opt().oco_extras.end())
        throw std::invalid_argument("Item not found!");
    if (oco_extra_.first == name) return;
    points_ -= oco_extra_.second;
    oco_extra_.first = name;
    oco_extra_.second = search->second.second;
    points_ += search->second.second;
}

void character_unit::pick_mc_extra(std::string name) {
    auto search = handle_->opt().mc_extras.find(name);
    if (search == handle_->opt().mc_extras.end())
        throw std::invalid_argument("Item not found!");
    if (mc_extras_.count(name)) return;
    mc_extras_[name] = search->second.second;
    points_ += search->second.second;
}

void character_unit::remove_weapon(WeaponType wt) {
    if (!weapons_.count(wt)) return;
    auto weapon = weapons_[wt];
    auto search = handle_->eq().weapons.find(wt);
    if (search != handle_->eq().weapons.cend()) { // avoid removing default weapon
        if (search->second.second == std::get<1>(weapon)) return;
        weapons_[wt] = {search->second.first, search->second.second, 0.0};
    }
    else weapons_.erase(wt);
    // remove points value of weapon
    const double pts = std::get<2>(weapon);
    switch (std::get<0>(weapon)) {
    case ItemClass::MUNDANE:
        break;
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    default: break;
    }
    points_ -= pts;
}

void character_unit::remove_armour(ArmourType at) {
    if (!armours_.count(at)) return;
    auto armour = armours_[at];
    auto search = handle_->eq().armour.find(at);
    if (search != handle_->eq().armour.cend()) { // avoid removing default armour
        if (search->second.second == std::get<1>(armour)) return;
        armours_[at] = {search->second.first, search->second.second, 0.0};
    }
    else armours_.erase(at);
    // remove points value of armour
    const double pts = std::get<2>(armour);
    switch (std::get<0>(armour)) {
    case ItemClass::MUNDANE:
        break;
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    default: break;
    }
    points_ -= pts;
}

void character_unit::remove_oco_extra() {
    points_ -= oco_extra_.second;
    oco_extra_.first = "";
    oco_extra_.second = 0.0;
}

void character_unit::remove_mc_extra(std::string name) {
    auto search = mc_extras_.find(name);
    if (search != mc_extras_.end()) {
        points_ -= search->second;
        mc_extras_.erase(name);
    }
}
