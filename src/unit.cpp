#include "unit.h"

unit::unit(std::shared_ptr<base_unit> base)
    : id_(0), model_select_(ModelSelect::DEFAULT),
      mixed_select_(MixedSelect::SLAVE), points_(0.0), base_(base) {}

unit::unit(const unit& other)
    : id_(other.id()), model_select_(other.model_select_),
      mixed_select_(other.mixed_select_),
      points_(other.points_), base_(other.base_) {}

int unit::id() const noexcept { return id_; }
void unit::set_id(int id) { id_ = id; }

ModelSelect unit::model_select() const noexcept { return model_select_; }
bool unit::switch_model_select(ModelSelect ms) {
    model_select_ = ms;
    return false;
}

MixedSelect unit::mixed_select() const noexcept { return mixed_select_; }
bool unit::switch_mixed_select(MixedSelect ms) {
    mixed_select_ = ms;
    return false;
}

double unit::points() const noexcept { return points_; }

const std::shared_ptr<base_unit>& unit::base() const noexcept {
    return base_;
}

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
