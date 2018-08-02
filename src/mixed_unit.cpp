#include "mixed_unit.h"

mixed_unit::mixed_unit(std::shared_ptr<base_unit> base)
    : unit(base),
      handle(std::dynamic_pointer_cast<base_mixed_unit>(base)),
      master_(std::make_shared<base_normal_unit>(handle->master())),
      slave_(std::make_shared<base_normal_unit>(handle->slave())),
      mixed_select_(MixedSelect::SLAVE) {}

normal_unit& mixed_unit::master() noexcept { return master_; }
normal_unit& mixed_unit::slave() noexcept { return slave_; }

double mixed_unit::points() const noexcept {
    return master_.points() + slave_.points();
}

std::unordered_map<
    WeaponType,
    std::tuple<ItemClass, std::string, double>
> mixed_unit::weapons() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.weapons();
    case MixedSelect::SLAVE:
        return slave_.weapons();
    }
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemClass, std::string, double>
> mixed_unit::armour() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.armour();
    case MixedSelect::SLAVE:
        return slave_.armour();
    }
}

std::pair<
    std::string,
    std::pair<bool, double>
> mixed_unit::oco_extra() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.oco_extra();
    case MixedSelect::SLAVE:
        return slave_.oco_extra();
    }
}

std::unordered_map<
    std::string,
    std::pair<bool, double>
> mixed_unit::mc_extras() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.mc_extras();
    case MixedSelect::SLAVE:
        return slave_.mc_extras();
    }
}

void mixed_unit::pick_weapon(ItemClass item_type, std::string name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.pick_weapon(item_type, name);
        break;
    case MixedSelect::SLAVE:
        slave_.pick_weapon(item_type, name);
        break;
    }
}

void mixed_unit::remove_weapon(WeaponType wt) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.remove_weapon(wt);
        break;
    case MixedSelect::SLAVE:
        slave_.remove_weapon(wt);
        break;
    }
}

void mixed_unit::pick_armour(ItemClass item_type, std::string name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.pick_armour(item_type, name);
        break;
    case MixedSelect::SLAVE:
        slave_.pick_armour(item_type, name);
        break;
    }
}

void mixed_unit::remove_armour(ArmourType at) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.remove_armour(at);
        break;
    case MixedSelect::SLAVE:
        slave_.remove_armour(at);
        break;
    }
}

void mixed_unit::pick_oco_extra(std::string name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.pick_oco_extra(name);
        break;
    case MixedSelect::SLAVE:
        slave_.pick_oco_extra(name);
        break;
    }
}

void mixed_unit::remove_oco_extra() {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.remove_oco_extra();
        break;
    case MixedSelect::SLAVE:
        slave_.remove_oco_extra();
        break;
    }
}

void mixed_unit::pick_mc_extra(std::string name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.pick_mc_extra(name);
        break;
    case MixedSelect::SLAVE:
        slave_.pick_mc_extra(name);
        break;
    }
}

void mixed_unit::remove_mc_extra(std::string name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.remove_mc_extra(name);
        break;
    case MixedSelect::SLAVE:
        slave_.remove_mc_extra(name);
        break;
    }
}