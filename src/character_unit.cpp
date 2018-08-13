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

character_unit::character_unit(const character_unit& other)
 : unit(other), weapons_(other.weapons_), armours_(other.armours_),
   talisman_(other.talisman_), enchanted_item_(other.enchanted_item_),
   oco_extra_(other.oco_extra_), mc_extras_(other.mc_extras_),
   magic_item_points_(other.magic_item_points_),
   faction_item_points_(other.faction_item_points_),
   total_item_points_(other.total_item_points_),
   handle_(other.handle_) {}

std::size_t character_unit::size() const noexcept { return 1U; }

std::unordered_map<
    WeaponType,
    std::tuple<ItemClass, std::string, double>
> character_unit::weapons() const noexcept { return weapons_; }

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> character_unit::armour() const noexcept { return armours_; }

std::pair<std::string, double> character_unit::talisman() const noexcept { return talisman_; }

std::pair<std::string, double> character_unit::enchanted_item() const noexcept { return enchanted_item_; }

std::pair<std::string, std::pair<bool, double>> character_unit::oco_extra() const noexcept { return oco_extra_; }
std::unordered_map<std::string, std::pair<bool, double>> character_unit::mc_extras() const noexcept { return mc_extras_; }

std::pair<
    std::string,
    std::pair<armies::UnitClass, double>
> character_unit::mount() const noexcept { return mount_; }

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
        auto search = handle_->faction_items_handle()->find(name);
        if (search == handle_->faction_items_handle()->end())
            throw std::invalid_argument("Weapon not found!");
        double fi_budget = handle_->faction_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + faction_item_points_ > fi_budget) ||
                (search->second.points + faction_item_points_ > ti_budget))
            throw std::runtime_error("Faction item budget exceeded!");
        // check if the faction weapon has specific allowed units
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
        faction_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::COMMON:
    {
        auto search = handle_->common_items_handle()->find(name);
        if (search == handle_->common_items_handle()->end())
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
        auto search = handle_->faction_items_handle()->find(name);
        if (search == handle_->faction_items_handle()->end())
            throw std::invalid_argument("Armour not found!");
        double fi_budget = handle_->faction_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + faction_item_points_ > fi_budget) ||
                (search->second.points + faction_item_points_ > ti_budget))
            throw std::runtime_error("Faction item budget exceeded!");
        // check if the faction weapon has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic weapons' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this armour!");
        }
        remove_armour(search->second.armour_type);
        armours_[search->second.armour_type] = {
            item_type,
            search->first,
            search->second.points
        };
        points_ += search->second.points;
        faction_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::COMMON:
    {
        auto search = handle_->common_items_handle()->find(name);
        if (search == handle_->common_items_handle()->end())
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
    }
}

void character_unit::pick_talisman(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    {
        auto search = handle_->magic_items_handle()->find(name);
        if (search == handle_->magic_items_handle()->end())
            throw std::runtime_error("Talisman not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the talisman has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic talismans' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this talisman!");
        }
        remove_talisman();
        talisman_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::COMMON:
    {
        auto search = handle_->common_items_handle()->find(name);
        if (search == handle_->common_items_handle()->end())
            throw std::runtime_error("Talisman not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the talisman has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic talismans' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this talisman!");
        }
        remove_talisman();
        talisman_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    default: break;
    }
}

void character_unit::remove_talisman() {
    points_ -= talisman_.second;
    magic_item_points_ -= talisman_.second;
    total_item_points_ -= talisman_.second;
    talisman_.first = "";
    talisman_.second = 0.0;
}

void character_unit::pick_enchanted_item(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    {
        auto search = handle_->magic_items_handle()->find(name);
        if (search == handle_->magic_items_handle()->end())
            throw std::runtime_error("Enchanted item not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the talisman has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic talismans' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this enchanted item!");
        }
        remove_enchanted_item();
        enchanted_item_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::COMMON:
    {
        auto search = handle_->common_items_handle()->find(name);
        if (search == handle_->common_items_handle()->end())
            throw std::runtime_error("Enchanted item not found!");
        double mi_budget = handle_->magic_item_budget();
        double ti_budget = handle_->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the item has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the items' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this enchanted item!");
        }
        remove_enchanted_item();
        enchanted_item_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    default: break;
    }
}

void character_unit::remove_enchanted_item() {
    points_ -= enchanted_item_.second;
    magic_item_points_ -= enchanted_item_.second;
    total_item_points_ -= enchanted_item_.second;
    enchanted_item_.first = "";
    enchanted_item_.second = 0.0;
}

void character_unit::pick_oco_extra(std::string name) {
    auto search = handle_->opt().oco_extras.find(name);
    if (search == handle_->opt().oco_extras.end())
        throw std::invalid_argument("Item not found!");
    if (oco_extra_.first == name) return;
    points_ -= oco_extra_.second.second;
    oco_extra_.first = name;
    oco_extra_.second = search->second;
    points_ += search->second.second;
}

void character_unit::pick_mc_extra(std::string name) {
    auto search = handle_->opt().mc_extras.find(name);
    if (search == handle_->opt().mc_extras.end())
        throw std::invalid_argument("Item not found!");
    if (mc_extras_.count(name)) return;
    mc_extras_[name] = search->second;
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
    }
    points_ -= pts;
}

void character_unit::remove_oco_extra() {
    points_ -= oco_extra_.second.second;
    oco_extra_.first = "";
    oco_extra_.second.second = 0.0;
}

void character_unit::remove_mc_extra(std::string name) {
    auto search = mc_extras_.find(name);
    if (search != mc_extras_.end()) {
        points_ -= search->second.second;
        mc_extras_.erase(name);
    }
}

void character_unit::pick_mount(std::string name) {
    auto search = handle_->opt().opt_mounts.find(name);
    if (search == handle_->opt().opt_mounts.end())
        throw std::invalid_argument("Mount not found!");
    if (mount_.first == name) return;
    points_ -= mount_.second.second;
    mount_.first = name;
    mount_.second = search->second;
    points_ += search->second.second;
}

void character_unit::remove_mount() {
    points_ -= mount_.second.second;
    mount_.first = "";
    mount_.second.first = handle_->unit_class();
    mount_.second.second = 0.0;
}
