#include "unit.h"
#include "../army_list.h"

unit::unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : id_(0), model_select_(ModelSelect::DEFAULT),
      mixed_select_(MixedSelect::SLAVE), points_(0.0),
      magic_item_points_(0.0),
      faction_item_points_(0.0),
      total_item_points_(0.0),
      n_magic_items(0U),
      base_(base), army_(army_handle) {}

unit::unit(const unit& other)
    : id_(other.id()), model_select_(other.model_select_),
      mixed_select_(other.mixed_select_),
      points_(other.points_),
      magic_item_points_(other.magic_item_points_),
      faction_item_points_(other.faction_item_points_),
      total_item_points_(other.total_item_points_),
      n_magic_items(other.n_magic_items),
      base_(other.base_), army_(other.army_) {}

int unit::id() const noexcept { return id_; }
void unit::set_id(int id) { id_ = id; }

const std::string& unit::assigned_name() const noexcept {
    return assigned_name_;
}
void unit::assign_name(std::string aname) {
    assigned_name_ = aname;
}

std::string unit::full_name() const noexcept {
    if (assigned_name_.empty())
        return name();
    return assigned_name_ + " (" + name() + ')';
}

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
    std::unordered_map<RestrictionField, short> each_restr_count;
    for (const auto& restriction : restrictions) each_restr_count[restriction.first]++;
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
                msg += "Cannot have more than " + std::to_string(lim) + " instances "
                        "of " + item_name + " in an army!";
            break;
        }
        // everything else handled by restriction_check calls in derived classes
        default: break;
        }
    }
    if (!msg.empty() && !each_restr_count[RestrictionField::LIMIT]) msg += " to choose: " + item_name;
    return msg;
}

std::string unit::budget_restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        ItemType item_type
) const {
    std::string msg = "";
    for (const auto& restriction : restrictions) {
        switch (restriction.first) {
        case RestrictionField::LIMIT:
        {
            unsigned int lim = std::any_cast<unsigned int>(restriction.second);
            unsigned int n = n_magic_items;
            switch (item_type) { // current item of corresponding type not removed yet
            case ItemType::WEAPON:
                for (const auto& w : weapons()) {
                    if (std::get<0>(w.second) != ItemCategory::MUNDANE
                            && std::get<0>(w.second) != ItemCategory::NONE) {
                        n -= 1;
                        break;
                    }
                }
                break;
            case ItemType::ARMOUR:
                for (const auto& a : armour()) {
                    if (std::get<0>(a.second) != ItemCategory::MUNDANE
                            && std::get<0>(a.second) != ItemCategory::NONE) {
                        n -= 1;
                        break;
                    }
                }
                break;
            case ItemType::TALISMAN:
                if (!talisman().first.empty()) n -= 1;
                break;
            case ItemType::ENCHANTED:
                if (!enchanted_item().first.empty()) n -= 1;
                break;
            case ItemType::ARCANE:
                if (!arcane_item().first.empty()) n -= 1;
                break;
            default: break;
            }
            if (!(n < lim))
                msg += name() + " can only have " + std::to_string(lim) + " magic item"
                       + (lim > 1 ? "s!" : "!");
            break;
        }
        case RestrictionField::ITEMTYPE:
        {
            std::vector<ItemType> v = std::any_cast<std::vector<ItemType>>(restriction.second);
            if (!std::count(std::begin(v), std::end(v), item_type))
                msg += name() + " cannot choose items of type: " + enum_convert::ITEM_TYPE_TO_STRING.at(item_type);
            break;
        }
        case RestrictionField::SUBITEMTYPE:
            break;
        default: break;
        }
    }
    return msg;
}

std::string unit::pick_magic_item(ItemType item_type, ItemCategory item_category, const std::string& name) {
    std::string removed = "";
    if (item_category == ItemCategory::MUNDANE || item_category == ItemCategory::NONE) return removed;
    if (is_character() && item_type != ItemType::BANNER && !magic_banner().first.empty())
        throw std::runtime_error("BSBs equipped with a Magic Banner cannot take Magic Items!");
    // obtain the iterator to the desired item
    std::unordered_map<std::string, item>::const_iterator search;
    if (item_category == ItemCategory::MAGIC) {
        if (base_->magic_items_handle() == nullptr) return removed;
        search = base_->magic_items_handle()->second.find(name);
        if (search == base_->magic_items_handle()->second.end())
            throw std::invalid_argument("Item not found!");
    } else if (item_category == ItemCategory::COMMON) {
        if (base_->common_items_handle() == nullptr) return removed;
        search = base_->common_items_handle()->second.find(name);
        if (search == base_->common_items_handle()->second.end())
            throw std::invalid_argument("Item not found!");
    } else {
        if (base_->faction_items_handle() == nullptr) return removed;
        search = base_->faction_items_handle()->second.find(name);
        if (search == base_->faction_items_handle()->second.end())
            throw std::invalid_argument("Item not found!");
    }
    // get the budget values...
    const double mi_budget = (item_category == ItemCategory::FACTION) ?
        base_->faction_item_budget().points : base_->magic_item_budget().points;
    const double ti_budget = base_->total_item_budget().points;
    // ... and the current magic item points values - to be used for adjusting
    // the budgets down to the allowed remaining points
    double adj_mip = (item_category == ItemCategory::FACTION) ? faction_item_points() : magic_item_points();
    double adj_tip = total_item_points();
    // do restriction checks on the budgets
    auto budget_restr_ind_msg = budget_restriction_check(
        (item_category == ItemCategory::FACTION) ?
            base_->faction_item_budget().restrictions : base_->magic_item_budget().restrictions,
        item_type
    );
    if (!budget_restr_ind_msg.empty()) throw std::invalid_argument(budget_restr_ind_msg);
    auto budget_restr_tot_msg = budget_restriction_check(
        base_->total_item_budget().restrictions,
        item_type
    );
    if (!budget_restr_tot_msg.empty()) throw std::invalid_argument(budget_restr_tot_msg);
    // do restriction checks on the item itself
    auto item_restr_msg = restriction_check(search->second.restrictions, name);
    if (!item_restr_msg.empty()) throw std::invalid_argument(item_restr_msg);
    // subtract current magic item points of matching item type if applicable
    switch (item_type) {
    case ItemType::WEAPON:
    {
        auto search_weapon = weapons().find(search->second.weapon_type);
        if (search_weapon != std::end(weapons())) {
            adj_mip -= std::get<2>(search_weapon->second);
            adj_tip -= std::get<2>(search_weapon->second);
        }
        break;
    }
    case ItemType::ARMOUR:
    {
        auto armour_type = search->second.armour_type;
        // protect against cases where the user attempts to pick another
        // piece of magic armour not in the category being altered
        if (std::count_if(std::begin(armour()), std::end(armour()), [armour_type] (const auto& a) {
            return (std::get<0>(a.second) == ItemCategory::COMMON ||
                    std::get<0>(a.second) == ItemCategory::MAGIC ||
                    std::get<0>(a.second) == ItemCategory::FACTION) && a.first != armour_type;
        })) throw std::invalid_argument("Cannot choose multiple pieces of magic armour!");
        auto search_armour = armour().find(search->second.armour_type);
        if (search_armour != std::end(armour())) {
            adj_mip -= std::get<2>(search_armour->second);
            adj_tip -= std::get<2>(search_armour->second);
        }
        break;
    }
    case ItemType::TALISMAN:
    {
        if (!talisman().first.empty()) {
            adj_mip -= talisman().second.second;
            adj_tip -= talisman().second.second;
        }
        break;
    }
    case ItemType::ENCHANTED:
    {
        if (!enchanted_item().first.empty()) {
            adj_mip -= enchanted_item().second.second;
            adj_tip -= enchanted_item().second.second;
        }
        break;
    }
    case ItemType::ARCANE:
    {
        if (!arcane_item().first.empty()) {
            adj_mip -= arcane_item().second.second;
            adj_tip -= arcane_item().second.second;
        }
        break;
    }
    case ItemType::BANNER:
    {
        if (!is_character()) {
            if (magic_banner().first.empty()) {
                if (search->second.points > base_->magic_banner_budget())
                    throw std::runtime_error("Magic banner budget exceeded!");
            } else {
                if (search->second.points > base_->magic_banner_budget() + magic_banner().second.second)
                    throw std::runtime_error("Magic banner budget exceeded!");
            }
        }
        break;
    }
    default: break;
    }
    // perform budget value checking now that we've adjusted for previous items
    if (item_type != ItemType::BANNER) {
        if ((search->second.points + adj_mip > mi_budget) ||
                (search->second.points + adj_tip > ti_budget))
            throw std::runtime_error("Item budget exceeded!");
    }
    // check if the item has specific allowed units
    if (!(search->second.allowed_units.empty())) {
        std::string unit_name = this->name();
        if (!std::count_if(
                std::begin(search->second.allowed_units),
                std::end(search->second.allowed_units),
                [&unit_name](const auto& x) { return x == unit_name; }
            )) throw std::invalid_argument(unit_name + " cannot take this item!");
    }
    // switch on the item_type once again and assign the new chosen item
    // whilst removing the previous one
    switch (item_type) {
    case ItemType::WEAPON:
        removed = remove_weapon(search->second.weapon_type);
        weapons_access()[search->second.weapon_type] = {
            item_category, search->first, search->second.points
        };
        break;
    case ItemType::ARMOUR:
        removed = remove_armour(search->second.armour_type);
        armour_access()[search->second.armour_type] = {
            item_category, search->first, search->second.points
        };
        break;
    case ItemType::TALISMAN:
        removed = remove_talisman();
        talisman_access() = {search->first, {item_category, search->second.points}};
        break;
    case ItemType::ENCHANTED:
        removed = remove_enchanted_item();
        enchanted_item_access() = {search->first, {item_category, search->second.points}};
        break;
    case ItemType::ARCANE:
        removed = remove_arcane_item();
        arcane_item_access() = {search->first, {item_category, search->second.points}};
        break;
    case ItemType::BANNER:
        removed = remove_banner();
        magic_banner_access() = {search->first, {item_category, search->second.points}};
        break;
    case ItemType::OTHER:
        item_extras_access()[search->first] = {item_category, search->second.points};
        break;
    default: break;
    }
    points_ += search->second.points;
    if (item_type != ItemType::BANNER) {
        ++n_magic_items;
        if (item_category == ItemCategory::FACTION)
            faction_item_points_ += search->second.points;
        else magic_item_points_ += search->second.points;
        total_item_points_ += search->second.points;
    }
    army_->incr_item_tracker(name);
    return removed;
}

std::string unit::remove_talisman() {
    auto name = talisman_access().first;
    auto talisman_pts = talisman_access().second.second;
    points_ -= talisman_pts;
    switch (talisman_access().second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= talisman_pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= talisman_pts;
        break;
    default: break;
    }
    total_item_points_ -= talisman_pts;
    talisman_access().first.clear();
    talisman_access().second.first = ItemCategory::NONE;
    talisman_access().second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(name);
    return name;
}

std::string unit::remove_enchanted_item() {
    auto name = enchanted_item_access().first;
    auto enchanted_item_pts = enchanted_item_access().second.second;
    points_ -= enchanted_item_pts;
    switch (enchanted_item_access().second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= enchanted_item_pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= enchanted_item_pts;
        break;
    default: break;
    }
    total_item_points_ -= enchanted_item_pts;
    enchanted_item_access().first.clear();
    enchanted_item_access().second.first = ItemCategory::NONE;
    enchanted_item_access().second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(name);
    return name;
}

std::string unit::remove_arcane_item() {
    auto name = arcane_item_access().first;
    auto arcane_item_pts = arcane_item_access().second.second;
    points_ -= arcane_item_pts;
    switch (arcane_item_access().second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= arcane_item_pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= arcane_item_pts;
        break;
    default: break;
    }
    total_item_points_ -= arcane_item_pts;
    arcane_item_access().first.clear();
    arcane_item_access().second.first = ItemCategory::NONE;
    arcane_item_access().second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(name);
    return name;
}

std::string unit::remove_magic_extra(const std::string& _name) {
    auto search = item_extras_access().find(_name);
    if (search == std::end(item_extras_access()))
        throw std::invalid_argument(name() + " does not have this item!");
    auto item_pts = search->second.second;
    points_ -= item_pts;
    switch (search->second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= item_pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= item_pts;
        break;
    default: break;
    }
    total_item_points_ -= item_pts;
    item_extras_access().erase(_name);
    --n_magic_items;
    army_->decr_item_tracker(_name);
    return _name;
}

double unit::points() const noexcept { return points_; }

double unit::magic_item_points() const noexcept {
    return magic_item_points_;
}

double unit::faction_item_points() const noexcept {
    return faction_item_points_;
}

double unit::total_item_points() const noexcept {
    return total_item_points_;
}

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

void unit::serialise_items(std::string& stream, std::string pre) {
    tools::serialise_weapon(weapons(), WeaponType::MELEE, stream, pre);
    tools::serialise_weapon(weapons(), WeaponType::BALLISTIC, stream, pre);
    tools::serialise_armour(armour(), ArmourType::ARMOUR, stream, pre);
    tools::serialise_armour(armour(), ArmourType::SHIELD, stream, pre);
    tools::serialise_armour(armour(), ArmourType::HELMET, stream, pre);
    tools::serialise_oco_extra(oco_extra(), stream, pre);
    tools::serialise_mc_extras(mc_extras(), stream, pre);
}

std::string unit::save() {
    const std::string NT = "\n\t";
    std::string serialised = name();
    serialised += NT + "ID = " + std::to_string(id());
    serialised += NT + "POINTS = " + std::to_string(points());
    serialised += NT + "MAGIC_ITEM_POINTS = " + std::to_string(magic_item_points());
    serialised += NT + "FACTION_ITEM_POINTS = " + std::to_string(faction_item_points());
    serialised += NT + "TOTAL_ITEM_POINTS = " + std::to_string(total_item_points());

    if (!is_character()) {
        if (!is_mixed()) {
            serialise_items(serialised, NT);
            switch_model_select(ModelSelect::CHAMPION);
            serialise_items(serialised, NT + "CHAMPION_");
        }
        else {
            serialise_items(serialised, NT + "MASTER_");
            switch_model_select(ModelSelect::CHAMPION);
            serialise_items(serialised, NT + "MASTER_CHAMPION_");
            switch_mixed_select(MixedSelect::SLAVE);
            serialise_items(serialised, NT + "SLAVE_CHAMPION_");
            switch_model_select(ModelSelect::DEFAULT);
            serialise_items(serialised, NT + "SLAVE_");
        }
    }
    else {
        serialise_items(serialised, NT);
    }

    tools::serialise_magic_item(talisman(), "TALISMAN", serialised, NT);
    tools::serialise_magic_item(enchanted_item(), "ENCHANTED_ITEM", serialised, NT);
    tools::serialise_magic_item(arcane_item(), "ARCANE_ITEM", serialised, NT);
    if (!item_extras().empty()) {
        serialised += NT + "MAGIC_ITEM_EXTRAS =";
        for (const auto& item_details : item_extras()) {
            serialised += " name: " + item_details.first + ", category: "
                          + enum_convert::ITEM_CLASS_TO_STRING.at(item_details.second.first) + ';';
        }
    }
    if (!magic_banner().first.empty()) {
        ItemCategory category = magic_banner().second.first;
        std::string name = magic_banner().first;
        serialised += NT + "BANNER = name: " + name + ", category: " +
                      enum_convert::ITEM_CLASS_TO_STRING.at(category);
    }
    tools::serialise_mount(mnt(), serialised, NT);

    return serialised;
}
