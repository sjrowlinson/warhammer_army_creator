#include "mage_character_unit.h"

mage_character_unit::mage_character_unit(std::shared_ptr<base_unit> base)
    : character_unit(base),
      handle(std::dynamic_pointer_cast<base_mage_character_unit>(base)) {}

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
