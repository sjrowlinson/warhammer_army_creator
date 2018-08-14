#include "mage_character_unit.h"

mage_character_unit::mage_character_unit(std::shared_ptr<base_unit> base)
    : character_unit(base),
      handle(std::dynamic_pointer_cast<base_mage_character_unit>(base)) {}

mage_character_unit::mage_character_unit(const mage_character_unit& other)
    : character_unit(other), level_(other.level_), arcane_item_(other.arcane_item_),
      handle(other.handle) {}

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

std::pair<std::string, double> mage_character_unit::arcane_item() const noexcept {
    return arcane_item_;
}

void mage_character_unit::pick_arcane_item(ItemClass item_class, std::string name) {
    switch (item_class) {
    case ItemClass::MAGIC:
    {
        auto search = handle->magic_items_handle()->second.find(name);
        if (search == handle->magic_items_handle()->second.end())
            throw std::runtime_error("Arcane item not found!");
        double mi_budget = handle->magic_item_budget();
        double ti_budget = handle->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
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
        arcane_item_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    case ItemClass::COMMON:
    {
        auto search = handle->common_items_handle()->second.find(name);
        if (search == handle->common_items_handle()->second.end())
            throw std::runtime_error("Arcane item not found!");
        double mi_budget = handle->magic_item_budget();
        double ti_budget = handle->total_item_budget();
        if ((search->second.points + magic_item_points_ > mi_budget) ||
                (search->second.points + total_item_points_ > ti_budget))
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
        arcane_item_ = {search->first, search->second.points};
        points_ += search->second.points;
        magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
        break;
    }
    default: break;
    }
}

void mage_character_unit::remove_arcane_item() {
    points_ -= arcane_item_.second;
    magic_item_points_ -= arcane_item_.second;
    total_item_points_ -= arcane_item_.second;
    arcane_item_.first = "";
    arcane_item_.second = 0.0;
}
