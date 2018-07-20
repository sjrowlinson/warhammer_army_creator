#include "_unit.h"

_unit::_unit(std::shared_ptr<_base_unit> base)
    : id_(0), base_(base) {}

int _unit::id() const noexcept { return id_; }
void _unit::set_id(int id) { id_ = id; }

double _unit::points() const noexcept { return points_; }

BaseUnitType _unit::base_unit_type() const noexcept {
    return base_->base_unit_type();
}

std::string _unit::name() const noexcept {
    return base_->name();
}

std::size_t _unit::min_size() const noexcept {
    return base_->min_size();
}

std::size_t _unit::max_size() const noexcept {
    return base_->max_size();
}

armies::Faction _unit::faction() const noexcept {
    return base_->faction();
}

armies::UnitType _unit::unit_type() const noexcept {
    return base_->unit_type();
}

armies::UnitClass _unit::unit_class() const noexcept {
    return base_->unit_class();
}
