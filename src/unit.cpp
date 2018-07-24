#include "unit.h"

unit::unit(std::shared_ptr<base_unit> base)
    : id_(0), points_(0.0), base_(base) {}

int unit::id() const noexcept { return id_; }
void unit::set_id(int id) { id_ = id; }

double unit::points() const noexcept { return points_; }

BaseUnitType unit::base_unit_type() const noexcept {
    return base_->base_unit_type();
}

std::string unit::name() const noexcept {
    return base_->name();
}

std::size_t unit::min_size() const noexcept {
    return base_->min_size();
}

std::size_t unit::max_size() const noexcept {
    return base_->max_size();
}

armies::Faction unit::faction() const noexcept {
    return base_->faction();
}

armies::UnitType unit::unit_type() const noexcept {
    return base_->unit_type();
}

armies::UnitClass unit::unit_class() const noexcept {
    return base_->unit_class();
}
