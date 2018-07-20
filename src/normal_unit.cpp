#include "normal_unit.h"

normal_unit::normal_unit(std::shared_ptr<_base_unit> base)
    : _unit(base),
      handle(std::dynamic_pointer_cast<base_normal_unit>(base)) {
    for (const auto& x : handle->eq().weapons) {
        auto p = x.second;
        weapons[x.first] = {p.first, p.second, 0.0};
    }
    for (const auto& x : handle->eq().armour) {
        auto p = x.second;
        armours[x.first] = {p.first, p.second, 0.0};
    }
}

const std::vector<short>& normal_unit::statistics() const noexcept {
    return handle->statistics();
}

const std::vector<short>& normal_unit::champion_statistics() const noexcept {
    return handle->champion_statistics();
}

const std::vector<std::string>& normal_unit::special_rules() const noexcept {
    return handle->special_rules();
}
