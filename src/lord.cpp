#include "lord.h"

lord::lord(armies::Faction faction,
           const std::string& name,
           std::size_t points,
           bool is_named=false)
    : unit(
        faction,
        armies::UnitType::LORD,
        name,
        points
    ), is_named(is_named) {}

lord::~lord() {}

bool lord::special_character() const noexcept { return is_named; }
