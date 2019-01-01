#include "options.h"

options::options()
    : weapons_(), armour_(), mounts_(), oco_extras_(), mc_extras_() {}

options::options(tmp_parse_options&& tpo)
    : weapons_(std::move(tpo.weapons)),
      armour_(std::move(tpo.armour)),
      mounts_(std::move(tpo.mounts)),
      oco_extras_(std::move(tpo.oco_extras)),
      mc_extras_(std::move(tpo.mc_extras)) {}

const std::unordered_map<std::string, weapon_option>& options::weapons() const noexcept {
    return weapons_;
}

const std::unordered_map<std::string, armour_option>& options::armour() const noexcept {
   return armour_;
}

const std::unordered_map<std::string, mount_option>& options::mounts() const noexcept {
    return mounts_;
}

const std::unordered_map<std::string, extra_option>& options::oco_extras() const noexcept {
    return oco_extras_;
}

const std::unordered_map<std::string, extra_option>& options::mc_extras() const noexcept {
    return mc_extras_;
}
