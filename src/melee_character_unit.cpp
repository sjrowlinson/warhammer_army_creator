#include "melee_character_unit.h"

melee_character_unit::melee_character_unit(std::shared_ptr<_base_unit> base)
    : _unit(base),
      handle(std::dynamic_pointer_cast<base_melee_character_unit>(base)) {
    points_ = handle->points();
    for (const auto& w : handle->eq().weapons) {
        auto p = w.second;
        weapons_[w.first] = {p.first, p.second, 0.0};
    }
    for (const auto& a : handle->eq().armour) {
        auto p = a.second;
        armours_[a.first] = {p.first, p.second, 0.0};
    }
    magic_item_points_ = 0.0;
    faction_item_points_ = 0.0;
    total_item_points_ = 0.0;
}

// current property accessors

std::size_t melee_character_unit::size() const noexcept { return 1U; }

std::tuple<ItemClass, std::string, double> melee_character_unit::melee_weapon() const {
    auto search = weapons_.find(WeaponType::MELEE);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}
std::tuple<ItemClass, std::string, double> melee_character_unit::ranged_weapon() const {
    auto search = weapons_.find(WeaponType::BALLISTIC);
    if (search != weapons_.end()) return (*search).second;
    return {ItemClass::MUNDANE, "", 0.0};
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> melee_character_unit::armour() const noexcept {
    return armours_;
}

std::unordered_map<
    std::string,
    double
> melee_character_unit::extras() const noexcept {
    return extras_;
}

void melee_character_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle->opt().opt_weapons.find(name);
        if (search == handle->opt().opt_weapons.end())
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
        auto search = handle->magic_items_handle()->find(name);
        if (search == handle->magic_items_handle()->end())
            throw std::invalid_argument("Weapon not found!");
        double mi_budget = handle->magic_item_budget();
        double ti_budget = handle->total_item_budget();
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

void melee_character_unit::pick_armour(ItemClass item_type, std::string name) {
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
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    {
        auto search = handle->magic_items_handle()->find(name);
        if (search == handle->magic_items_handle()->end())
            throw std::invalid_argument("Armour not found!");
        double mi_budget = handle->magic_item_budget();
        double ti_budget = handle->total_item_budget();
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

void melee_character_unit::pick_extra(std::string name) {
    auto search = handle->opt().opt_extras.first.find(name);
    if (search == handle->opt().opt_extras.first.end())
        throw std::invalid_argument("Item not found!");
    if (handle->opt().opt_extras.second) { // one choice only
        for (const auto& x : extras_) {
            points_ -= x.second;
        }
        extras_.clear();
    }
    extras_[search->first] = search->second.second;
    points_ += search->second.second;
}

void melee_character_unit::remove_weapon(WeaponType wt) {
    auto weapon = weapons_[wt];
    auto def_w = handle->eq().weapons.at(wt);
    if (def_w.second != std::get<1>(weapon)) {
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
        weapons_[wt] = {def_w.first, def_w.second, 0.0};
    }
}

void melee_character_unit::remove_armour(ArmourType at) {
    auto armour = armours_[at];
    auto def_a = handle->eq().armour.at(at);
    if (def_a.second != std::get<1>(armour)) {
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
        armours_[at] = {def_a.first, def_a.second, 0.0};
    }
}

void melee_character_unit::remove_extra(std::string name) {
    auto search = extras_.find(name);
    if (search != extras_.end()) {
        points_ -= search->second;
        extras_.erase(name);
    }
}
