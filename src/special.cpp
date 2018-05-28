#include "special.h"

special::special(armies::Faction faction,
                 const std::string& name,
                 std::size_t points,
                 std::size_t size,
                 std::size_t min_size)
    : unit(
        faction,
        armies::UnitType::SPECIAL,
        name,
        points,
        size,
        min_size
    ) {}

special::~special() {}
