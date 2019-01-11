#include "mage_character_unit.h"

mage_character_unit::mage_character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : character_unit(base, army_handle),
      handle(std::dynamic_pointer_cast<base_mage_character_unit>(base)) {
    level_ = {handle->mage_level(), 0.0};
}

mage_character_unit::mage_character_unit(const mage_character_unit& other)
    : character_unit(other), level_(other.level_), arcane_item_(other.arcane_item_),
      handle(other.handle) {}

std::string mage_character_unit::restriction_check(
    const std::unordered_multimap<RestrictionField, std::any>& restrictions,
    const std::string& item_name
) const {
    std::string msg = character_unit::restriction_check(restrictions, item_name);
    for (const auto& restriction : restrictions) {
        switch (restriction.first) {
        case RestrictionField::ARCANE:
        case RestrictionField::LORE:
            for (const auto& x : std::any_cast<std::vector<std::string>>(restriction.second)) {
                bool restricted = false;
                switch (restriction.first) {
                case RestrictionField::ARCANE:
                    restricted = arcane_item().first != x;
                    break;
                case RestrictionField::LORE:
                    restricted = std::none_of(std::begin(lores_), std::end(lores_), [&x](const auto& y) { return y == x; });
                    break;
                default: break;
                }
                if (restricted) {
                    if (msg.empty()) msg += name() + " requires: " + x;
                    else msg += ", " + x;
                }
            }
            break;
        default: break;
        }
    }
    return msg;
}

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

const std::pair<std::string, std::pair<ItemCategory, double>>& mage_character_unit::arcane_item() const noexcept {
    return arcane_item_;
}

std::string mage_character_unit::pick_arcane_item(ItemCategory item_class, const std::string& name) {
    std::string removed;
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    {
        std::unordered_map<std::string, item>::const_iterator search;
        switch (item_class) {
        case ItemCategory::MAGIC:
            search = handle_->magic_items_handle()->second.find(name);
            if (search == handle_->magic_items_handle()->second.end())
                throw std::invalid_argument("Arcane item not found!");
            break;
        case ItemCategory::COMMON:
            search = handle_->common_items_handle()->second.find(name);
            if (search == handle_->common_items_handle()->second.end())
                throw std::invalid_argument("Arcane item not found!");
            break;
        default: break;
        }
        const double mi_budget = handle->magic_item_budget().points;
        const double ti_budget = handle->total_item_budget().points;
        double adj_mip = magic_item_points_;
        double adj_tip = total_item_points_;
        auto restr = restriction_check(search->second.restrictions, name);
        if (!restr.empty()) throw std::invalid_argument(restr);
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
        removed = remove_arcane_item();
        arcane_item_ = {search->first, {item_class, search->second.points}};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemCategory::FACTION:
    {
        auto search = handle_->faction_items_handle()->second.find(name);
        if (search == handle_->faction_items_handle()->second.end())
            throw std::invalid_argument("Arcane item not found!");
        const double fi_budget = handle_->faction_item_budget().points;
        const double ti_budget = handle_->total_item_budget().points;
        double adj_fip = faction_item_points_;
        double adj_tip = total_item_points_;
        auto restr = restriction_check(search->second.restrictions, name);
        if (!restr.empty()) throw std::invalid_argument(restr);
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
        removed = remove_arcane_item();
        arcane_item_ = {search->first, {item_class, search->second.points}};
        points_ += search->second.points;
        faction_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    default: break;
    }
    return removed;
}

std::string mage_character_unit::remove_arcane_item() {
    std::string removed = arcane_item_.first;
    points_ -= arcane_item_.second.second;
    switch (arcane_item_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= arcane_item_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= arcane_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= arcane_item_.second.second;
    arcane_item_.first.clear();
    arcane_item_.second.second = 0.0;
    return removed;
}

const std::vector<std::string>& mage_character_unit::lores() const noexcept {
    return lores_;
}
void mage_character_unit::pick_lore(const std::string& lore) {
    auto search = std::find_if(std::begin(handle->lores()), std::end(handle->lores()), [&lore](const auto& x) {
        return x.name == lore;
    });
    if (search == std::end(handle->lores()))
        throw std::invalid_argument(name() + " cannot choose this Lore of Magic!");
    auto restr = restriction_check(search->restrictions, "Lore: " + lore);
    if (!restr.empty()) throw std::invalid_argument(restr);
    lores_.push_back(lore);
}
void mage_character_unit::remove_lore(const std::string& lore) {
    lores_.erase(std::remove(std::begin(lores_), std::end(lores_), lore));
}

std::vector<std::string> mage_character_unit::clear() {
    auto removed = character_unit::clear();
    // remove arcane item
    auto arcane_rmvd = remove_arcane_item();
    if (!arcane_rmvd.empty()) removed.push_back(arcane_rmvd);
    return removed;
}

std::string mage_character_unit::html_table_row() const {
    return html_table_row_both(level_.first, arcane_item_.first);
}
