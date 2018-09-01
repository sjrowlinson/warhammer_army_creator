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
   item_extras_(other.item_extras_), mount_(other.mount_),
   magic_item_points_(other.magic_item_points_),
   faction_item_points_(other.faction_item_points_),
   total_item_points_(other.total_item_points_),
   handle_(other.handle_) {}

bool character_unit::is_character() const noexcept { return true; }
bool character_unit::is_mage() const noexcept { return false; }
bool character_unit::is_mixed() const noexcept { return false; }

bool character_unit::is_bsb() const noexcept {
    return mc_extras_.count(std::string("Battle Standard Bearer"));
}

void character_unit::pick_magic_item(ItemType item_type, ItemClass item_class, const std::string& name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
    {
        std::unordered_map<std::string, item>::const_iterator search;
        if (item_class == ItemClass::MAGIC) {
            search = handle_->magic_items_handle()->second.find(name);
            if (search == handle_->magic_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        } else if (item_class == ItemClass::COMMON) {
            search = handle_->common_items_handle()->second.find(name);
            if (search == handle_->common_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        } else {
            search = handle_->faction_items_handle()->second.find(name);
            if (search == handle_->faction_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        }
        const double mi_budget =
            (item_class == ItemClass::FACTION) ? handle_->faction_item_budget() : handle_->magic_item_budget();
        const double ti_budget = handle_->total_item_budget();
        double adj_mip =
            (item_class == ItemClass::FACTION) ? faction_item_points_ : magic_item_points_;
        double adj_tip = total_item_points_;
        switch (item_type) {
        case ItemType::WEAPON:
            if (weapons_.find(search->second.weapon_type) != weapons_.end()) {
                adj_mip -= std::get<2>(weapons_[search->second.weapon_type]);
                adj_tip -= std::get<2>(weapons_[search->second.weapon_type]);
            }
            break;
        case ItemType::ARMOUR:
        {
            // protect against cases where user tries to pick another piece of
            // magic armour not in the category being changed
            auto at = search->second.armour_type;
            if (item_class == ItemClass::COMMON || item_class == ItemClass::MAGIC) {
                if(std::count_if(
                    std::begin(armours_),
                    std::end(armours_),
                    [at](const auto& a) {
                        return (std::get<0>(a.second) == ItemClass::COMMON ||
                                std::get<0>(a.second) == ItemClass::MAGIC) &&
                                a.first != at;
                    }
                )) throw std::invalid_argument("Cannot choose multiple pieces of magic armour!");
            }
            if (armours_.find(at) != armours_.end()) {
                adj_mip -= std::get<2>(armours_[at]);
                adj_tip -= std::get<2>(armours_[at]);
            }
            break;
        }
        case ItemType::TALISMAN:
            if (!talisman_.first.empty()) {
                adj_mip -= talisman_.second.second;
                adj_tip -= talisman_.second.second;
            }
            break;
        case ItemType::ENCHANTED:
            if (!enchanted_item_.first.empty()) {
                adj_mip -= enchanted_item_.second.second;
                adj_tip -= enchanted_item_.second.second;
            }
            break;
        case ItemType::BANNER:
            break;
        default: break;
        }
        if (item_type != ItemType::BANNER) {
            if ((search->second.points + adj_mip > mi_budget) ||
                    (search->second.points + adj_tip > ti_budget))
                throw std::runtime_error("Item budget exceeded!");
        }
        // check if the item has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the items; allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this item!");
        }
        switch (item_type) {
        case ItemType::WEAPON:
            remove_weapon(search->second.weapon_type);
            weapons_[search->second.weapon_type] = {
                item_class,
                search->first,
                search->second.points
            };
            break;
        case ItemType::ARMOUR:
            remove_armour(search->second.armour_type);
            armours_[search->second.armour_type] = {
                item_class,
                search->first,
                search->second.points
            };
            break;
        case ItemType::TALISMAN:
            remove_talisman();
            talisman_ = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::ENCHANTED:
            remove_enchanted_item();
            enchanted_item_ = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::BANNER:
            remove_banner();
            banner = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::OTHER:
            item_extras_[search->first] = {item_class, search->second.points};
            break;
        default: break;
        }
        points_ += search->second.points;
        if (item_type != ItemType::BANNER) {
            if (item_class == ItemClass::FACTION) faction_item_points_ += search->second.points;
            else magic_item_points_ += search->second.points;
            total_item_points_ += search->second.points;
        }
        break;
    }
    case ItemClass::MUNDANE: break;
    }
}

std::size_t character_unit::size() const noexcept { return 1U; }

std::unordered_map<
    WeaponType,
    std::tuple<ItemClass, std::string, double>
> character_unit::weapons() const noexcept { return weapons_; }

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> character_unit::armour() const noexcept { return armours_; }

std::pair<std::string, std::pair<ItemClass, double>> character_unit::talisman() const noexcept { return talisman_; }

std::pair<std::string, std::pair<ItemClass, double>> character_unit::enchanted_item() const noexcept { return enchanted_item_; }

const std::unordered_map<std::string, std::pair<ItemClass, double>>& character_unit::item_extras() const noexcept {
    return item_extras_;
}

std::pair<std::string, std::pair<bool, double>> character_unit::oco_extra() const noexcept { return oco_extra_; }
std::unordered_map<std::string, std::pair<bool, double>> character_unit::mc_extras() const noexcept { return mc_extras_; }

std::pair<
    std::string,
    std::pair<armies::UnitClass, double>
> character_unit::mount() const noexcept { return mount_; }

std::pair<std::string, std::pair<ItemClass, double>> character_unit::magic_banner() const noexcept {
    return banner;
}

void character_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch (item_type) {
    case ItemClass::MUNDANE:
    {
        auto search = handle_->opt().opt_weapons.find(name);
        if (search == handle_->opt().opt_weapons.cend())
            throw std::invalid_argument("Weapon not found!");
        remove_weapon(std::get<0>(search->second));
        do_replacements(std::get<3>(search->second));
        weapons_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::WEAPON, item_type, name);
        break;
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
        do_replacements(std::get<3>(search->second));
        armours_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::ARMOUR, item_type, name);
        break;
    }
}

void character_unit::pick_talisman(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::TALISMAN, item_class, name);
        break;
    default: break;
    }
}

void character_unit::remove_talisman() {
    points_ -= talisman_.second.second;
    switch (talisman_.second.first) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= talisman_.second.second;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= talisman_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= talisman_.second.second;
    talisman_.first.clear();
    talisman_.second.second = 0.0;
}

void character_unit::pick_enchanted_item(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::ENCHANTED, item_class, name);
        break;
    default: break;
    }
}

void character_unit::remove_enchanted_item() {
    points_ -= enchanted_item_.second.second;
    switch (enchanted_item_.second.first) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= enchanted_item_.second.second;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= enchanted_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= enchanted_item_.second.second;
    enchanted_item_.first.clear();
    enchanted_item_.second.second = 0.0;
}

void character_unit::pick_other(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::OTHER, item_class, name);
        break;
    default: break;
    }
}

void character_unit::remove_other(std::string name) {
    auto search = item_extras_.find(name);
    if (search == item_extras_.end())
        throw std::invalid_argument("Unit does not have this item!");
    points_ -= search->second.second;
    switch (search->second.first) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= search->second.second;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= search->second.second;
        break;
    default: break;
    }
    total_item_points_ -= search->second.second;
    item_extras_.erase(name);
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

void character_unit::remove_weapon(WeaponType wt, bool replacing) {
    if (!weapons_.count(wt)) return;
    auto weapon = weapons_[wt];
    auto search = handle_->eq().weapons.find(wt);
    if (search != handle_->eq().weapons.cend()) {
        if (replacing) {
            weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                weapons_[wt] = {ItemClass::MUNDANE, "Hand weapon", 0.0};
        }
        else {
            if (search->second.second == std::get<1>(weapon)) return;
            weapons_[wt] = {search->second.first, search->second.second, 0.0};
        }
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

void character_unit::remove_armour(ArmourType at, bool replacing) {
    if (!armours_.count(at)) return;
    auto armour = armours_[at];
    auto search = handle_->eq().armour.find(at);
    if (search != handle_->eq().armour.cend()) {
        if (replacing) {
            armours_.erase(at);
        } else {
            if (search->second.second == std::get<1>(armour)) return;
            armours_[at] = {search->second.first, search->second.second, 0.0};
        }
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

void character_unit::pick_banner(ItemClass item_class, std::string name) {
    if (!is_bsb())
        throw std::runtime_error("Only the Battle Standard Bearer may take a Magic Standard!");
    if (is_bsb() && (!talisman_.first.empty() || !enchanted_item_.first.empty() ||
                     !item_extras_.empty() ||
                     std::count_if(
                        std::begin(weapons_), std::end(weapons_),
                        [](const auto& w) {
                            return (std::get<0>(w.second) == ItemClass::COMMON ||
                                    std::get<0>(w.second) == ItemClass::MAGIC);
                        }
                     ) ||
                     std::count_if(
                        std::begin(armours_), std::end(armours_),
                        [](const auto& a) {
                            return (std::get<0>(a.second) == ItemClass::COMMON ||
                                    std::get<0>(a.second) == ItemClass::MAGIC);
                        }
                     ))
       ) throw std::runtime_error("BSBs equipped with Magic Equipment cannot take Magic Standards!");
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    case ItemClass::FACTION:
        pick_magic_item(ItemType::BANNER, item_class, name);
        break;
    default: break;
    }
}

void character_unit::remove_banner() {
    points_ -= banner.second.second;
    switch (enchanted_item_.second.first) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= banner.second.second;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= banner.second.second;
        break;
    default: break;
    }
    total_item_points_ -= banner.second.second;
    banner.first.clear();
    banner.second.second = 0.0;
}

std::string character_unit::html_table_row_both(short mlevel, std::string arcane) const {
    std::string row = "<tr>";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit mount
    row += "<td>" + (mount_.first.empty() ? "&nbsp;" : mount_.first) + "</td>\n";
    // unit mage level
    if (mlevel == -1) row += "<td>None</td>\n";
    else row += "<td>Level " + std::to_string(mlevel) + "</td>\n";
    // weapons
    if (weapons_.count(WeaponType::MELEE))
        row += "<td><strong>Melee:</strong> " +
                std::get<1>(weapons_.at(WeaponType::MELEE)) +
                (weapons_.count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (weapons_.count(WeaponType::BALLISTIC))
        row +=  std::string(weapons_.count(WeaponType::MELEE) ? "" : "<td>") + "<strong>Ranged:</strong> " +
                std::get<1>(weapons_.at(WeaponType::BALLISTIC)) + "</td>\n";
    // armour
    if (armours_.count(ArmourType::ARMOUR))
        row += "<td><strong>Body:</strong> " +
                std::get<1>(armours_.at(ArmourType::ARMOUR)) +
                (armours_.count(ArmourType::SHIELD) || armours_.count(ArmourType::HELMET)
                    ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::SHIELD))
        row +=  std::string(armours_.count(ArmourType::ARMOUR) ? "" : "<td>") + "<strong>Shield:</strong> " +
                std::get<1>(armours_.at(ArmourType::SHIELD)) +
                (armours_.count(ArmourType::HELMET) ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::HELMET))
        row +=  std::string((armours_.count(ArmourType::ARMOUR) || armours_.count(ArmourType::SHIELD))
                            ? "" : "<td>") + "<strong>Helmet:</strong> " +
                std::get<1>(armours_.at(ArmourType::HELMET)) + "</td>\n";
    if (!armours_.count(ArmourType::ARMOUR) &&
            !armours_.count(ArmourType::SHIELD) &&
            !armours_.count(ArmourType::HELMET)) row += "<td>&nbsp;</td>";
    // talisman
    row += "<td>";
    if (!talisman_.first.empty()) row += talisman_.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // enchanted item
    row += "<td>";
    if (!enchanted_item_.first.empty()) row += enchanted_item_.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // arcane item
    if (arcane.empty()) row += "<td>&nbsp;</td>\n";
    else row += "<td>" + arcane + "</td>\n";
    // magic/faction item extras
    row += "<td>";
    for (const auto& x : item_extras_) row += x.first + "<br/>";
    if (item_extras_.empty()) row += "&nbsp;";
    row += "</td>\n";
    // other extras
    row += "<td>";
    if (!oco_extra_.first.empty()) row += oco_extra_.first + (mc_extras().empty() ? "" : "<br/>");
    for (const auto& x : mc_extras_) row += x.first + "<br/>";
    if (oco_extra_.first.empty() && mc_extras_.empty()) row += "&nbsp;";
    row += "</td>\n";
    // banner (if BSB)
    row += "<td>";
    if (!banner.first.empty()) row += banner.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // characteristics
    row += "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    row += "<thead><tr>\n"
            "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
            "<th>I</th><th>A</th><th>Ld</th>\n"
           "</tr></thead>\n";
    row += "<tr>\n";
    for (const auto& x : handle_->statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    row += "</table></td>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}

std::string character_unit::html_table_row() const {
    return html_table_row_both(-1, std::string());
}
