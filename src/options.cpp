#include "options.h"

options::options()
    : weapons_(), armour_(), mounts_(), oco_extras_(), mc_extras_() {}

options::options(
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
    >&& _weapons,
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
    >&& _armours,
    std::unordered_map<
        std::string,
        double
    >&& _mounts,
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    >&& _oco_extras,
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    >&& _mc_extras
) : weapons_(std::move(_weapons)),
    armour_(std::move(_armours)),
    mounts_(std::move(_mounts)),
    oco_extras_(std::move(_oco_extras)),
    mc_extras_(std::move(_mc_extras)) {}

options::options(
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    >&& _oco_extras,
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    >&& _mc_extras
) : weapons_(), armour_(), mounts_(),
    oco_extras_(std::move(_oco_extras)),
    mc_extras_(std::move(_mc_extras)) {}

const std::unordered_map<
    std::string,
    std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
>& options::weapons() const noexcept {
    return weapons_;
}
const std::unordered_map<
    std::string,
    std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
>& options::armour() const noexcept {
    return armour_;
}
const std::unordered_map<
    std::string,
    double
>& options::mounts() const noexcept {
    return mounts_;
}
const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& options::oco_extras() const noexcept {
    return oco_extras_;
}
const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& options::mc_extras() const noexcept {
    return mc_extras_;
}
