#include "unit.h"
#include "army_list.h"

unit::unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : id_(0), model_select_(ModelSelect::DEFAULT),
      mixed_select_(MixedSelect::SLAVE), points_(0.0), n_magic_items(0U),
      base_(base), army_(army_handle) {}

unit::unit(const unit& other)
    : id_(other.id()), model_select_(other.model_select_),
      mixed_select_(other.mixed_select_),
      points_(other.points_), n_magic_items(other.n_magic_items),
      base_(other.base_), army_(other.army_) {}

int unit::id() const noexcept { return id_; }
void unit::set_id(int id) { id_ = id; }

ModelSelect unit::model_select() const noexcept { return model_select_; }
bool unit::switch_model_select(ModelSelect ms) {
    model_select_ = ms;
    return false;
}

MixedSelect unit::mixed_select() const noexcept { return mixed_select_; }
bool unit::switch_mixed_select(MixedSelect ms) {
    mixed_select_ = ms;
    return false;
}

bool unit::has_non_duplicable_items() const noexcept {
    return false;
}

void unit::do_replacements(const std::vector<std::string>& replacements, bool champion) {
    if (champion) switch_model_select(ModelSelect::CHAMPION);
    for (const auto& r : replacements) {
        if (r == "Melee") remove_weapon(WeaponType::MELEE, true);
        else if (r == "Ballistic") remove_weapon(WeaponType::BALLISTIC, true);
        else if (r == "Body") remove_armour(ArmourType::ARMOUR, true);
        else if (r == "Shield") remove_armour(ArmourType::SHIELD, true);
        else if (r == "Helmet") remove_armour(ArmourType::HELMET, true);
    }
    switch_model_select(ModelSelect::DEFAULT);
}

std::string unit::restriction_check(
    const std::unordered_multimap<RestrictionField, std::any>& restrictions,
    const std::string& item_name
) const {
    std::string msg = "";
    /*std::unordered_map<RestrictionField, short> each_restr_count;
    for (const auto& restriction : restrictions) each_restr_count[restriction.first]++;*/
    for (const auto& restriction : restrictions) {
        switch (restriction.first) {
        case RestrictionField::WEAPON:
        case RestrictionField::ARMOUR:
        case RestrictionField::MC_EXTRA:
        case RestrictionField::ARMY_CONTAINS:
            for (const auto& x : std::any_cast<std::vector<std::string>>(restriction.second)) {
                bool restricted = false;
                switch (restriction.first) {
                case RestrictionField::WEAPON:
                    restricted = std::none_of(std::begin(weapons()), std::end(weapons()),
                                              [&x](const auto& y) { return std::get<1>(y.second) == x; });

                    break;
                case RestrictionField::ARMOUR:
                    restricted = std::none_of(std::begin(armour()), std::end(armour()),
                                              [&x](const auto& y) { return std::get<1>(y.second) == x; });
                    break;
                case RestrictionField::MC_EXTRA:
                    restricted = !(mc_extras().count(x));
                    break;
                case RestrictionField::ARMY_CONTAINS:
                    restricted = !(army_->contains(x));
                    if (restricted) {
                        if (msg.empty()) msg += "Army must contain: " + x;
                        else msg += ", " + x;
                    }
                    break;
                default: break;
                }
                if (restricted && restriction.first != RestrictionField::ARMY_CONTAINS) {
                    if (msg.empty()) msg += name() + " requires: " + x;
                    else msg += ", " + x;
                }
            }
            break;
        case RestrictionField::BANNER:
        {
            std::string reqd_banner = std::any_cast<std::string>(restriction.second);
            if (magic_banner().first != reqd_banner) {
                if (msg.empty()) msg += name() + " requires: " + reqd_banner;
                else msg += ", " + reqd_banner;
            }
            break;
        }
        case RestrictionField::MOUNT:
        {
            std::string reqd_mount = std::any_cast<std::string>(restriction.second);
            if (std::get<0>(mnt()) != reqd_mount) {
                if (msg.empty()) msg += name() + " requires: " + reqd_mount;
                else msg += ", " + reqd_mount;
            }
            break;
        }
        case RestrictionField::OCO_EXTRA:
        {
            std::string reqd_oco = std::any_cast<std::string>(restriction.second);
            if (oco_extra().first != reqd_oco) {
                if (msg.empty()) msg += name() + " requires: " + reqd_oco;
                else msg += ", " + reqd_oco;
            }
            break;
        }
        case RestrictionField::LIMIT:
        {
            unsigned int lim = std::any_cast<unsigned int>(restriction.second);
            auto search_item_in_tracker = army_->item_track_map().find(item_name);
            if (search_item_in_tracker != std::end(army_->item_track_map()) &&
                    search_item_in_tracker->second >= lim)
                msg += "Cannot have more than " + std::to_string(lim) + " instances"
                        "of " + item_name + " in an army!";
            break;
        }
        case RestrictionField::ITEMTYPE:
        case RestrictionField::SUBITEMTYPE:
            // TODO: don't know what to do with these yet, need to rewrite
            // budget and item parsing first
            break;
        // everything else handled by restriction_check calls in derived classes
        default: break;
        }
    }
    if (!msg.empty()) msg += " to choose: " + item_name;
    return msg;
}

std::string unit::budget_restriction_check(
    const std::unordered_multimap<RestrictionField, std::any>& restrictions
) const {
    std::string msg = "";
    for (const auto& restriction : restrictions) {
        switch (restriction.first) {
        case RestrictionField::LIMIT:
        {
            unsigned int lim = std::any_cast<unsigned int>(restriction.second);
            if (!(n_magic_items < lim))
                msg += name() + " can only have " + std::to_string(lim) + " magic item"
                       + (lim > 1 ? "s!" : "!");
            break;
        }
        default: break;
        }
    }
    return msg;
}

double unit::points() const noexcept { return points_; }

const std::shared_ptr<base_unit>& unit::base() const noexcept {
    return base_;
}

BaseUnitType unit::base_unit_type() const noexcept {
    return base_->base_unit_type();
}

const std::string& unit::name() const noexcept {
    return base_->name();
}

std::size_t unit::min_size() const noexcept {
    return base_->min_size();
}

std::size_t unit::max_size() const noexcept {
    return base_->max_size();
}

Faction unit::faction() const noexcept {
    return base_->faction();
}

UnitType unit::unit_type() const noexcept {
    return base_->unit_type();
}

UnitCategory unit::unit_class() const noexcept {
    return base_->unit_class();
}

void unit::pick_mount_option(const std::string& name, bool oco) {
    (void)(name);
    (void)(oco);
    return;
}
void unit::remove_mount_option(const std::string& name, bool oco) {
    (void)(name);
    (void)(oco);
    return;
}

std::vector<std::string> unit::clear() {
    std::vector<std::string> removed;
    // remove weapons
    auto melee_rmvd = remove_weapon(WeaponType::MELEE);
    if (!melee_rmvd.empty()) removed.push_back(melee_rmvd);
    auto ballistic_rmvd = remove_weapon(WeaponType::BALLISTIC);
    if (!ballistic_rmvd.empty()) removed.push_back(ballistic_rmvd);
    // remove armour
    auto body_rmvd = remove_armour(ArmourType::ARMOUR);
    if (!body_rmvd.empty()) removed.push_back(body_rmvd);
    auto shield_rmvd = remove_armour(ArmourType::SHIELD);
    if (!shield_rmvd.empty()) removed.push_back(shield_rmvd);
    auto helmet_rmvd = remove_armour(ArmourType::HELMET);
    if (!helmet_rmvd.empty()) removed.push_back(helmet_rmvd);
    // remove oco extra
    auto oco_rmvd = remove_oco_extra();
    if (!oco_rmvd.empty()) removed.push_back(oco_rmvd);
    // remove mc extras
    if (!mc_extras().empty()) {
        std::vector<std::string> mc_extra_names(mc_extras().size());
        std::generate(
            std::begin(mc_extra_names), std::end(mc_extra_names),
            [it=std::cbegin(mc_extras())]() mutable { return (it++)->first; }
        );
        for (const auto& name : mc_extra_names) {
            auto mc_rmvd = remove_mc_extra(name);
            if (!mc_rmvd.empty()) removed.push_back(mc_rmvd);
        }
    }
    // remove banner
    auto banner_rmvd = remove_banner();
    if (!banner_rmvd.empty()) removed.push_back(banner_rmvd);
    // remove mount
    remove_mount();
    return removed;
}

