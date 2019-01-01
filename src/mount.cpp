#include "mount.h"

mount::mount()
    : name_(), unit_class_(UnitCategory::INFANTRY), statistics_(), special_rules_() {}

mount::mount(
    std::string&& _name,
    UnitCategory _uc,
    std::vector<std::string>&& _stats,
    std::vector<std::string>&& _sr
) : name_(std::move(_name)),
    unit_class_(_uc),
    statistics_(std::move(_stats)),
    special_rules_(std::move(_sr)) {}

const std::string& mount::name() const noexcept {
    return name_;
}
UnitCategory mount::unit_class() const noexcept {
    return unit_class_;
}
const std::vector<std::string>& mount::statistics() const noexcept {
    return statistics_;
}
const std::vector<std::string>& mount::special_rules() const noexcept {
    return special_rules_;
}

