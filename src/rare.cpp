#include "rare.h"

rare::rare(armies::Faction faction,
           const std::string& name,
           std::size_t points,
           std::size_t size,
           std::size_t min_size)
    : unit(
        faction,
        armies::UnitType::RARE,
        name,
        points,
        size,
        min_size
    ) {}

rare::~rare() {}
