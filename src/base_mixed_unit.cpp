#include "base_mixed_unit.h"

base_mixed_unit::base_mixed_unit(
    const std::string& name,
    base_normal_unit&& master,
    base_normal_unit&& slave
) : base_unit(
    slave.faction(),
    slave.unit_type(),
    slave.unit_class(),
    name,
    master.magic_item_budget(),
    master.faction_item_budget(),
    master.total_item_budget(),
    slave.min_size(),
    slave.max_size()
), ms_ratio_limit_(static_cast<double>(master.min_size())/slave.min_size()),
    master_(std::move(master)), slave_(std::move(slave))
 { but = BaseUnitType::MIXED; }

const base_normal_unit& base_mixed_unit::master() const noexcept {
    return master_;
}

const base_normal_unit& base_mixed_unit::slave() const noexcept {
    return slave_;
}

double base_mixed_unit::ratio_limit() const noexcept {
    return ms_ratio_limit_;
}
