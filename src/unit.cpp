#include "unit.h"

unit::unit(armies::Faction faction,
           armies::UnitType unit_type,
           const std::string& name,
           std::size_t points_per_model,
           std::size_t size=0U,
           std::size_t min_size=1U)
    : race(faction),
      unit_type(unit_type),
      name(name),
      points_per_model(points_per_model),
      size(size),
      min_size(min_size) {}

std::size_t unit::points_value() const noexcept {
    return points_per_model * size;
}

armies::UnitType unit::get_type() const noexcept {
    return unit_type;
}

const std::string& unit::get_name() const noexcept { return name; }

std::size_t unit::unit_size() const noexcept { return size; }

std::size_t unit::minimum_unit_size() const noexcept { return min_size; }

void unit::set_stat_table(std::array<short, 9U>&& stats) { stat_table = std::move(stats); }

void unit::set_equipment(std::vector<std::pair<std::string, std::string>>&& eq) {
    equipment = std::move(eq);
}

void unit::set_special_rules(std::vector<std::pair<std::string, std::string>>&& sr) {
    special_rules = std::move(sr);
}
