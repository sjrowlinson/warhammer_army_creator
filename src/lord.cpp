#include "lord.h"

lord::lord(const std::string& name, std::size_t points, bool is_named=false)
    : unit(name, points), is_named(is_named) {}

lord::~lord() {}

bool lord::special_character() const noexcept { return is_named; }
