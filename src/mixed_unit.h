#ifndef MIXED_UNIT_H
#define MIXED_UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "normal_unit.h"
#include "_base_unit.h"
#include "_unit.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

// TODO: need to redesign this and it likely won't work as intended
class mixed_unit : public _unit {
public:
    // handle
    const std::shared_ptr<base_mixed_unit> handle;
private:
    std::size_t master_size_;
    std::size_t slaves_size_;

    normal_unit master_;
    normal_unit slave_;
public:    
    explicit mixed_unit(std::shared_ptr<_base_unit> base);
    ~mixed_unit() = default;

    normal_unit& master() noexcept;
    normal_unit& slave() noexcept;

    double points() const noexcept override;
};

#endif // !MIXED_UNIT_H