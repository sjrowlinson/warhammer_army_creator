#include "base_unit.h"

base_unit::base_unit(
    armies::Faction faction,
    armies::UnitType ut,
    armies::UnitClass uc,
    std::string name,
    std::size_t min_size,
    std::size_t max_size
) : but(BaseUnitType::BASE), faction_(faction), ut_(ut), uc_(uc), name_(name),
    min_size_(min_size), max_size_(max_size) {}

BaseUnitType base_unit::base_unit_type() const noexcept { return but; }

armies::Faction base_unit::faction() const noexcept { return faction_; }
armies::UnitType base_unit::unit_type() const noexcept { return ut_; }
armies::UnitClass base_unit::unit_class() const noexcept { return uc_; }

std::string base_unit::name() const noexcept { return name_; }
std::size_t base_unit::min_size() const noexcept { return min_size_; }
std::size_t base_unit::max_size() const noexcept { return max_size_; }

const std::shared_ptr<
    std::unordered_map<std::string, magic_item>
>& base_unit::magic_items_handle() const noexcept {
    return magic_items;
}
void base_unit::set_magic_item_handle(
    const std::shared_ptr<
        std::unordered_map<
            std::string,
            magic_item
        >
    >& items
) noexcept { magic_items = items; }
