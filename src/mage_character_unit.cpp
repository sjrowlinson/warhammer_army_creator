#include "mage_character_unit.h"

mage_character_unit::mage_character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : character_unit(base, army_handle),
      handle(std::dynamic_pointer_cast<base_mage_character_unit>(base)) {
    level_ = {handle->mage_level(), 0.0};
}

mage_character_unit::mage_character_unit(const mage_character_unit& other)
    : character_unit(other), level_(other.level_),
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
