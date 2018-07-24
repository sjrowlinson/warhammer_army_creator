#ifndef BASE_MIXED_UNIT_H
#define BASE_MIXED_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_normal_unit.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_mixed_unit : public base_unit {
private:
    base_normal_unit master_;
    base_normal_unit slave_;
    std::size_t min_masters_per_slave_;
    std::size_t max_masters_per_slave_;
public:
    base_mixed_unit(
        base_normal_unit&& master,
        base_normal_unit&& slave,
        std::size_t min_masters_per_slave,
        std::size_t max_masters_per_slave
    );
    ~base_mixed_unit() = default;

    const base_normal_unit& master() const noexcept;
    const base_normal_unit& slave() const noexcept;

    std::size_t min_masters_per_slave() const noexcept;
    std::size_t max_masters_per_slave() const noexcept;
};

#endif // !BASE_MIXED_UNIT_H