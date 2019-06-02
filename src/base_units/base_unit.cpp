#include "base_unit.h"

base_unit::base_unit(Faction faction,
    UnitType ut,
    UnitCategory uc,
    const std::string& name,
    const budget &mi_budget,
    const budget &fi_budget,
    const budget &ti_budget,
    std::size_t min_size,
    std::size_t max_size,
    double banner_budget
) : but(BaseUnitType::BASE), faction_(faction), ut_(ut), uc_(uc), name_(name),
    magic_item_budget_(mi_budget), faction_item_budget_(fi_budget),
    total_item_budget_(ti_budget), magic_banner_budget_(banner_budget),
    min_size_(min_size), max_size_(max_size) {}

BaseUnitType base_unit::base_unit_type() const noexcept { return but; }

Faction base_unit::faction() const noexcept { return faction_; }
UnitType base_unit::unit_type() const noexcept { return ut_; }
UnitCategory base_unit::unit_class() const noexcept { return uc_; }

const std::string& base_unit::name() const noexcept { return name_; }
std::size_t base_unit::min_size() const noexcept { return min_size_; }
std::size_t base_unit::max_size() const noexcept { return max_size_; }

const budget& base_unit::magic_item_budget() const noexcept { return magic_item_budget_; }
const budget& base_unit::faction_item_budget() const noexcept { return faction_item_budget_; }
const budget& base_unit::total_item_budget() const noexcept { return total_item_budget_; }
double base_unit::magic_banner_budget() const noexcept { return magic_banner_budget_; }

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
