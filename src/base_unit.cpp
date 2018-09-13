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
    std::unordered_map<
        std::string, mount
    >
>& base_unit::mounts_handle() const noexcept { return mounts; }
void base_unit::set_mounts_handle(
    const std::shared_ptr<
       std::unordered_map<
           std::string, mount
       >
    >& mts
        ) noexcept {
    mounts = mts;
}

const std::shared_ptr<
    std::pair<
        std::string,
        std::unordered_map<std::string, item>
    >
>& base_unit::magic_items_handle() const noexcept {
    return magic_items;
}

const std::shared_ptr<
    std::pair<
        std::string,
        std::unordered_map<std::string, item>
    >
>& base_unit::common_items_handle() const noexcept {
    return common_items;
}

const std::shared_ptr<
    std::pair<
        std::string,
        std::unordered_map<std::string, item>
    >
>& base_unit::faction_items_handle() const noexcept {
    return faction_items;
}

void base_unit::set_magic_item_handle(
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& items
) noexcept { magic_items = items; }

void base_unit::set_common_item_handle(
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& items
) noexcept { common_items = items; }

void base_unit::set_faction_item_handle(
    const std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    >& items
) noexcept { faction_items = items; }
