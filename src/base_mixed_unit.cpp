#include "base_mixed_unit.h"

base_mixed_unit::base_mixed_unit(
    base_normal_unit&& master,
    base_normal_unit&& slave,
    std::size_t min_masters_per_slave,
    std::size_t max_masters_per_slave
) : _base_unit(
    master.faction(),
    master.unit_type(),
    master.unit_class(),
    master.name(),
    slave.min_size(),
    slave.max_size()
), master_(std::move(master)), slave_(std::move(slave)),
   min_masters_per_slave_(min_masters_per_slave),
   max_masters_per_slave_(max_masters_per_slave) { but = BaseUnitType::MIXED; }

const base_normal_unit& base_mixed_unit::master() const noexcept {
    return master_;
}

const base_normal_unit& base_mixed_unit::slave() const noexcept {
    return slave_;
}

std::size_t base_mixed_unit::min_masters_per_slave() const noexcept {
    return min_masters_per_slave_;
}

std::size_t base_mixed_unit::max_masters_per_slave() const noexcept {
    return max_masters_per_slave_;
}
