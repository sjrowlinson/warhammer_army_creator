#ifndef BASE_MIXED_UNIT_H
#define BASE_MIXED_UNIT_H

#include "base_unit.h"
#include "base_normal_unit.h"

#include <string>
#include <unordered_map>
#include <vector>

class base_mixed_unit : public base_unit {
private:
    double ms_ratio_limit_;
    base_normal_unit master_;
    base_normal_unit slave_;
public:
    explicit base_mixed_unit(
        const std::string& name,
        base_normal_unit&& master,
        base_normal_unit&& slave
    );
    ~base_mixed_unit() = default;

    const base_normal_unit& master() const noexcept;
    const base_normal_unit& slave() const noexcept;

    double ratio_limit() const noexcept;
};

#endif // !BASE_MIXED_UNIT_H
