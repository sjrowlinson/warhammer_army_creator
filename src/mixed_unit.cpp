#include "mixed_unit.h"

mixed_unit::mixed_unit(std::shared_ptr<base_unit> base)
    : unit(base),
      handle(std::dynamic_pointer_cast<base_mixed_unit>(base)),
      master_(std::make_shared<base_normal_unit>(handle->master())),
      slave_(std::make_shared<base_normal_unit>(handle->slave())) {}

normal_unit& mixed_unit::master() noexcept { return master_; }
normal_unit& mixed_unit::slave() noexcept { return slave_; }

double mixed_unit::points() const noexcept {
    return master_.points() + slave_.points();
}
