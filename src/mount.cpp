#include "mount.h"

mount::mount()
    : name_(), unit_class_(UnitClass::INFANTRY), statistics_(),
      special_rules_(), oco_extras_(), mc_extras_(), restrictions_() {}

mount::mount(
    std::string&& _name,
    UnitClass _uc,
    std::vector<std::string>&& _stats,
    std::vector<std::string>&& _sr,
    std::unordered_map<std::string, double>&& _oco_extras,
    std::unordered_map<std::string, double>&& _mc_extras,
    std::unordered_map<RestrictionField, std::vector<std::string>>&& _restrictions
) : name_(std::move(_name)),
    unit_class_(_uc),
    statistics_(std::move(_stats)),
    special_rules_(std::move(_sr)),
    oco_extras_(std::move(_oco_extras)),
    mc_extras_(std::move(_mc_extras)),
    restrictions_(std::move(_restrictions)) {}

const std::string& mount::name() const noexcept {
    return name_;
}
UnitClass mount::unit_class() const noexcept {
    return unit_class_;
}
const std::vector<std::string>& mount::statistics() const noexcept {
    return statistics_;
}
const std::vector<std::string>& mount::special_rules() const noexcept {
    return special_rules_;
}
const std::unordered_map<std::string, double>& mount::oco_extras() const noexcept {
    return oco_extras_;
}
const std::unordered_map<std::string, double>& mount::mc_extras() const noexcept {
    return mc_extras_;
}
const std::unordered_map<
    RestrictionField,
    std::vector<std::string>
>& mount::restrictions() const noexcept {
    return restrictions_;
}

bool mount::has_options() const noexcept {
    return !oco_extras_.empty() || !mc_extras_.empty();
}
