#include "mage_character_unit.h"

mage_character_unit::mage_character_unit(std::shared_ptr<base_unit> base)
    : character_unit(base),
      handle(std::dynamic_pointer_cast<base_mage_character_unit>(base)) {
    level_ = {handle->mage_level(), 0.0};
}

mage_character_unit::mage_character_unit(const mage_character_unit& other)
    : character_unit(other), level_(other.level_), arcane_item_(other.arcane_item_),
      handle(other.handle) {}

bool mage_character_unit::is_mage() const noexcept { return true; }

short mage_character_unit::level() const noexcept {
    return level_.first;
}

void mage_character_unit::change_level(short lvl) {
    auto search = handle->level_upgrades().find(lvl);
    if (search == handle->level_upgrades().end())
        throw std::invalid_argument("Level upgrade not available!");
    points_ += (search->second - level_.second);
    level_ = *search;
}

void mage_character_unit::reset_level() {
    points_ -= level_.second;
    level_ = {handle->mage_level(), 0.0};
}

std::pair<std::string, std::pair<ItemClass, double>> mage_character_unit::arcane_item() const noexcept {
    return arcane_item_;
}

void mage_character_unit::pick_arcane_item(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
    {
        std::unordered_map<std::string, item>::const_iterator search;
        switch (item_class) {
        case ItemClass::MAGIC:
            search = handle_->magic_items_handle()->second.find(name);
            if (search == handle_->magic_items_handle()->second.end())
                throw std::invalid_argument("Arcane item not found!");
            break;
        case ItemClass::COMMON:
            search = handle_->common_items_handle()->second.find(name);
            if (search == handle_->common_items_handle()->second.end())
                throw std::invalid_argument("Arcane item not found!");
            break;
        default: break;
        }
        const double mi_budget = handle->magic_item_budget();
        const double ti_budget = handle->total_item_budget();
        double adj_mip = magic_item_points_;
        double adj_tip = total_item_points_;
        if (!arcane_item_.first.empty()) {
            adj_mip -= arcane_item_.second.second;
            adj_tip -= arcane_item_.second.second;
        }
        if ((search->second.points + adj_mip > mi_budget) ||
                (search->second.points + adj_tip > ti_budget))
            throw std::runtime_error("Magic item budget exceeded!");
        // check if the arcane item has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the arcane items' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this enchanted item!");
        }
        remove_arcane_item();
        arcane_item_ = {search->first, {item_class, search->second.points}};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::FACTION:
    {
        auto search = handle_->faction_items_handle()->second.find(name);
        if (search == handle_->faction_items_handle()->second.end())
            throw std::invalid_argument("Arcane item not found!");
        const double fi_budget = handle_->faction_item_budget();
        const double ti_budget = handle_->total_item_budget();
        double adj_fip = faction_item_points_;
        double adj_tip = total_item_points_;
        if (!arcane_item_.first.empty()) {
            adj_fip -= arcane_item_.second.second;
            adj_tip -= arcane_item_.second.second;
        }
        if ((search->second.points + adj_fip > fi_budget) ||
                (search->second.points + adj_tip > ti_budget))
            throw std::runtime_error("Faction item budget exceeded!");
        // check if the faction arcane item has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the magic items' allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this arcane item!");
        }
        remove_arcane_item();
        arcane_item_ = {search->first, {item_class, search->second.points}};
        points_ += search->second.points;
        faction_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    default: break;
    }
}

void mage_character_unit::remove_arcane_item() {
    points_ -= arcane_item_.second.second;
    switch (arcane_item_.second.first) {
    case ItemClass::MAGIC:
    case ItemClass::COMMON:
        magic_item_points_ -= arcane_item_.second.second;
        break;
    case ItemClass::FACTION:
        faction_item_points_ -= arcane_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= arcane_item_.second.second;
    arcane_item_.first.clear();
    arcane_item_.second.second = 0.0;
}

std::string mage_character_unit::html_table_row() const {
    std::string row = "<tr>";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit mount
    row += "<td>" + (mount_.first.empty() ? "&nbsp;" : mount_.first) + "</td>\n";
    // unit mage level
    row += "<td>Level " + std::to_string(level_.first) + "</td>\n";
    // weapons
    if (weapons_.count(WeaponType::MELEE))
        row += "<td>Melee: " +
                std::get<1>(weapons_.at(WeaponType::MELEE)) +
                (weapons_.count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (weapons_.count(WeaponType::BALLISTIC))
        row +=  std::string(weapons_.count(WeaponType::MELEE) ? "" : "<td>") + "Ranged: " +
                std::get<1>(weapons_.at(WeaponType::BALLISTIC)) + "</td>\n";
    // armour
    if (armours_.count(ArmourType::ARMOUR))
        row += "<td>Body: " +
                std::get<1>(armours_.at(ArmourType::ARMOUR)) +
                (armours_.count(ArmourType::SHIELD) || armours_.count(ArmourType::HELMET)
                    ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::SHIELD))
        row +=  std::string(armours_.count(ArmourType::ARMOUR) ? "" : "<td>") + "Shield: " +
                std::get<1>(armours_.at(ArmourType::SHIELD)) +
                (armours_.count(ArmourType::HELMET) ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::HELMET))
        row +=  std::string((armours_.count(ArmourType::ARMOUR) || armours_.count(ArmourType::SHIELD))
                            ? "" : "<td>") + "Shield: " +
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
    row += "<td>";
    if (!arcane_item_.first.empty()) row += arcane_item_.first;
    else row += "&nbsp";
    row += "</td>\n";
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
    row += "<td>";
    for (const auto& x : handle_->statistics()) row += x + " ";
    row += "</td>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}
