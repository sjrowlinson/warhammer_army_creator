#include "character_unit.h"

character_unit::character_unit(const std::shared_ptr<base_unit>& base)
    : unit(base),
      handle_(std::dynamic_pointer_cast<base_character_unit>(base)) {
    points_ = handle_->points();
    for (const auto& w : handle_->eq().weapons()) {
        auto p = w.second;
        weapons_[w.first] = {p.first, p.second, 0.0};
    }
    for (const auto& a : handle_->eq().armours()) {
        auto p = a.second;
        armours_[a.first] = {p.first, p.second, 0.0};
    }
    magic_item_points_ = 0.0;
    faction_item_points_ = 0.0;
    total_item_points_ = 0.0;
}

character_unit::character_unit(const character_unit& other)
 : unit(other), weapons_(other.weapons_), armours_(other.armours_),
   talisman_(other.talisman_), enchanted_item_(other.enchanted_item_),
   oco_extra_(other.oco_extra_), mc_extras_(other.mc_extras_),
   item_extras_(other.item_extras_), mount_(other.mount_),
   magic_item_points_(other.magic_item_points_),
   faction_item_points_(other.faction_item_points_),
   total_item_points_(other.total_item_points_),
   handle_(other.handle_) {}

bool character_unit::is_character() const noexcept { return true; }
bool character_unit::is_mage() const noexcept { return false; }
bool character_unit::is_mixed() const noexcept { return false; }

bool character_unit::is_bsb() const noexcept {
    return mc_extras_.count(std::string("Battle Standard Bearer"));
}

bool character_unit::has_non_duplicable_items() const noexcept {
    return std::count_if(
        std::begin(weapons_),
        std::end(weapons_),
        [](const auto& w) {
            return std::get<0>(w.second) != ItemCategory::MUNDANE;
        }
    ) ||
    std::count_if(
        std::begin(armours_),
        std::end(armours_),
        [](const auto& a) {
            return std::get<0>(a.second) != ItemCategory::MUNDANE;
        }
    ) || !talisman_.first.empty() || !enchanted_item_.first.empty() ||
    std::count_if(
        std::begin(item_extras_),
        std::end(item_extras_),
        [](const auto& e) {
            return std::get<0>(e.second) != ItemCategory::MUNDANE;
        }
    ) || !banner.first.empty();
}

std::pair<bool, std::string> character_unit::restriction_check(
    RestrictionField picking,
    const std::unordered_map<RestrictionField, std::vector<std::string>>& restrictions
) const {
    std::pair<bool, std::string> p = {false, ""};
    for (const auto& restr : restrictions) {
        switch (restr.first) {
        case RestrictionField::WEAPON:
            for (const auto& w : restr.second) {
                if (std::none_of(std::begin(weapons_), std::end(weapons_),
                                 [&w](const auto& x) { return std::get<1>(x.second) == w; })
                        ) {
                    if (p.first) p.second += ", " + w;
                    else p.second += "Character requires: " + w;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::ARMOUR:
            for (const auto& a : restr.second) {
                if (std::none_of(std::begin(armours_), std::end(armours_),
                                 [&a](const auto& x) { return std::get<1>(x.second) == a; })
                        ) {
                    if (p.first) p.second += ", " + a;
                    else p.second += "Character requires: " + a;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::TALISMAN:
            for (const auto& t : restr.second) {
                if (talisman_.first != t) {
                    if (p.first) p.second += ", " + t;
                    else p.second += "Character requires: " + t;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::ENCHANTED:
            for (const auto& e : restr.second) {
                if (enchanted_item_.first != e) {
                    if (p.first) p.second += ", " + e;
                    else p.second += "Character requires: " + e;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::BANNER:
            for (const auto& b : restr.second) {
                if (banner.first != b) {
                    if (p.first) p.second += ", " + b;
                    else p.second += "Character requires: " + b;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::MOUNT:
            for (const auto& m : restr.second) {
                if (std::get<0>(mount_).name() != m) {
                    if (p.first) p.second += ", " + m;
                    else p.second += "Character requires: " + m;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::OCO_EXTRA:
            for (const auto& r : restr.second) {
                if (oco_extra_.first != r) {
                    if (p.first) p.second += ", " + r;
                    else p.second += "Character requires: " + r;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::MC_EXTRA:
            for (const auto& r : restr.second) {
                if (!mc_extras_.count(r)) {
                    if (p.first) p.second += ", " + r;
                    else p.second += "Character requires: " + r;
                    p.first = true;
                }
            }
            break;
        case RestrictionField::OTHER:
            for (const auto& o : restr.second) {
                if (!item_extras_.count(o)) {
                    if (p.first) p.second += ", " + o;
                    else p.second += "Character requires: " + o;
                    p.first = true;
                }
            }
            break;
        default: break;
        }
    }
    if (p.first)
        p.second += " to choose this " + enum_convert::RESTRICTION_TO_STRING.at(picking) + "!";
    return p;
}

std::string character_unit::pick_magic_item(ItemType item_type, ItemCategory item_class, const std::string& name) {
    if (item_type != ItemType::BANNER && !banner.first.empty())
        throw std::runtime_error("BSBs equipped with a Magic Standard cannot take Magic Items!");
    std::string removed = "";
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
    {
        std::unordered_map<std::string, item>::const_iterator search;
        if (item_class == ItemCategory::MAGIC) {
            search = handle_->magic_items_handle()->second.find(name);
            if (search == handle_->magic_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        } else if (item_class == ItemCategory::COMMON) {
            search = handle_->common_items_handle()->second.find(name);
            if (search == handle_->common_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        } else {
            search = handle_->faction_items_handle()->second.find(name);
            if (search == handle_->faction_items_handle()->second.end())
                throw std::invalid_argument("Item not found!");
        }
        const double mi_budget =
            (item_class == ItemCategory::FACTION) ? handle_->faction_item_budget() : handle_->magic_item_budget();
        const double ti_budget = handle_->total_item_budget();
        double adj_mip =
            (item_class == ItemCategory::FACTION) ? faction_item_points_ : magic_item_points_;
        double adj_tip = total_item_points_;
        switch (item_type) {
        case ItemType::WEAPON:
        {
            auto restr = restriction_check(RestrictionField::WEAPON, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            if (weapons_.find(search->second.weapon_type) != weapons_.end()) {
                adj_mip -= std::get<2>(weapons_[search->second.weapon_type]);
                adj_tip -= std::get<2>(weapons_[search->second.weapon_type]);
            }
            break;
        }
        case ItemType::ARMOUR:
        {
            // protect against cases where user tries to pick another piece of
            // magic armour not in the category being changed
            auto at = search->second.armour_type;
            if (item_class == ItemCategory::COMMON || item_class == ItemCategory::MAGIC) {
                if(std::count_if(
                    std::begin(armours_),
                    std::end(armours_),
                    [at](const auto& a) {
                        return (std::get<0>(a.second) == ItemCategory::COMMON ||
                                std::get<0>(a.second) == ItemCategory::MAGIC) &&
                                a.first != at;
                    }
                )) throw std::invalid_argument("Cannot choose multiple pieces of magic armour!");
            }
            auto restr = restriction_check(RestrictionField::ARMOUR, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            if (armours_.find(at) != armours_.end()) {
                adj_mip -= std::get<2>(armours_[at]);
                adj_tip -= std::get<2>(armours_[at]);
            }
            break;
        }
        case ItemType::TALISMAN:
        {
            auto restr = restriction_check(RestrictionField::TALISMAN, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            if (!talisman_.first.empty()) {
                adj_mip -= talisman_.second.second;
                adj_tip -= talisman_.second.second;
            }
            break;
        }
        case ItemType::ENCHANTED:
        {
            auto restr = restriction_check(RestrictionField::ENCHANTED, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            if (!enchanted_item_.first.empty()) {
                adj_mip -= enchanted_item_.second.second;
                adj_tip -= enchanted_item_.second.second;
            }
            break;
        }
        case ItemType::BANNER:
        {
            auto restr = restriction_check(RestrictionField::BANNER, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            break;
        }
        case ItemType::OTHER:
        {
            auto restr = restriction_check(RestrictionField::OTHER, search->second.restrictions);
            if (restr.first) throw std::invalid_argument(restr.second);
            break;
        }
        default: break;
        }
        if (item_type != ItemType::BANNER) {
            if ((search->second.points + adj_mip > mi_budget) ||
                    (search->second.points + adj_tip > ti_budget))
                throw std::runtime_error("Item budget exceeded!");
        }
        // check if the item has specific allowed units
        if (!(search->second.allowed_units.empty())) {
            std::string unit_name = this->name();
            // check that this unit is within the items; allowed units container
            if (!std::count_if(
                    std::begin(search->second.allowed_units),
                    std::end(search->second.allowed_units),
                    [&unit_name](const auto& x) { return x == unit_name; }
                )) throw std::invalid_argument("Character cannot take this item!");
        }
        switch (item_type) {
        case ItemType::WEAPON:
            removed = remove_weapon(search->second.weapon_type);
            weapons_[search->second.weapon_type] = {
                item_class,
                search->first,
                search->second.points
            };
            break;
        case ItemType::ARMOUR:
            removed = remove_armour(search->second.armour_type);
            armours_[search->second.armour_type] = {
                item_class,
                search->first,
                search->second.points
            };
            break;
        case ItemType::TALISMAN:
            removed = remove_talisman();
            talisman_ = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::ENCHANTED:
            removed = remove_enchanted_item();
            enchanted_item_ = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::BANNER:
            removed = remove_banner();
            banner = {search->first, {item_class, search->second.points}};
            break;
        case ItemType::OTHER:
            item_extras_[search->first] = {item_class, search->second.points};
            break;
        default: break;
        }
        points_ += search->second.points;
        if (item_type != ItemType::BANNER) {
            if (item_class == ItemCategory::FACTION) faction_item_points_ += search->second.points;
            else magic_item_points_ += search->second.points;
            total_item_points_ += search->second.points;
        }
        break;
    }
    case ItemCategory::MUNDANE:
    case ItemCategory::NONE:
        break;
    }
    return removed;
}

std::size_t character_unit::size() const noexcept { return 1U; }

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::weapons() const noexcept { return weapons_; }

const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::armour() const noexcept { return armours_; }

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::talisman() const noexcept {
    return talisman_;
}

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::enchanted_item() const noexcept {
    return enchanted_item_;
}

const std::unordered_map<std::string, std::pair<ItemCategory, double>>& character_unit::item_extras() const noexcept {
    return item_extras_;
}

const std::pair<std::string, std::pair<bool, double>>& character_unit::oco_extra() const noexcept {
    return oco_extra_;
}
const std::unordered_map<std::string, std::pair<bool, double>>& character_unit::mc_extras() const noexcept {
    return mc_extras_;
}

const std::tuple<
    mount,
    double,
    std::pair<std::string, double>,
    std::unordered_map<std::string, double>
>& character_unit::mnt() const noexcept { return mount_; }

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::magic_banner() const noexcept {
    return banner;
}

std::string character_unit::pick_weapon(ItemCategory item_type, const std::string& name) {
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle_->opt().weapons().find(name);
        if (search == handle_->opt().weapons().cend())
            throw std::invalid_argument("Weapon not found!");
        removed = remove_weapon(std::get<0>(search->second));
        do_replacements(std::get<3>(search->second));
        weapons_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::WEAPON, item_type, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::pick_armour(ItemCategory item_type, const std::string& name) {
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle_->opt().armour().find(name);
        if (search == handle_->opt().armour().cend())
            throw std::invalid_argument("Armour not found!");
        removed = remove_armour(std::get<0>(search->second));
        do_replacements(std::get<3>(search->second));
        armours_[std::get<0>(search->second)] = {
            std::get<1>(search->second),
            search->first,
            std::get<2>(search->second)
        };
        points_ += std::get<2>(search->second);
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::ARMOUR, item_type, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::pick_talisman(ItemCategory item_class, const std::string& name) {
    std::string removed;
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::TALISMAN, item_class, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::remove_talisman() {
    std::string removed = talisman_.first;
    points_ -= talisman_.second.second;
    switch (talisman_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= talisman_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= talisman_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= talisman_.second.second;
    talisman_.first.clear();
    talisman_.second.second = 0.0;
    return removed;
}

std::string character_unit::pick_enchanted_item(ItemCategory item_class, const std::string& name) {
    std::string removed;
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::ENCHANTED, item_class, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::remove_enchanted_item() {
    std::string removed = enchanted_item_.first;
    points_ -= enchanted_item_.second.second;
    switch (enchanted_item_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= enchanted_item_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= enchanted_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= enchanted_item_.second.second;
    enchanted_item_.first.clear();
    enchanted_item_.second.second = 0.0;
    return removed;
}

std::string character_unit::pick_other(ItemCategory item_class, const std::string& name) {
    std::string removed;
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::OTHER, item_class, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::remove_other(const std::string& name) {
    auto search = item_extras_.find(name);
    if (search == item_extras_.end())
        throw std::invalid_argument("Unit does not have this item!");
    points_ -= search->second.second;
    switch (search->second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= search->second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= search->second.second;
        break;
    default: break;
    }
    total_item_points_ -= search->second.second;
    item_extras_.erase(name);
    return name;
}

std::string character_unit::pick_oco_extra(const std::string& name) {
    std::string removed;
    auto search = handle_->opt().oco_extras().find(name);
    if (search == handle_->opt().oco_extras().end())
        throw std::invalid_argument("Item not found!");
    if (oco_extra_.first == name) return removed;
    removed = oco_extra_.first;
    if (handle_->faction() == Faction::SKAVEN) {
        if (name.find("Warpstone Tokens")) {
            const double mi_budget = handle_->magic_item_budget();
            const double ti_budget = handle_->total_item_budget();
            double adj_mip = magic_item_points_;
            double adj_tip = total_item_points_;
            if (oco_extra_.first.find("Warpstone Tokens")) {
                adj_mip -= oco_extra_.second.second;
                adj_tip -= oco_extra_.second.second;
            }
            if (search->second.second + adj_mip > mi_budget ||
                    search->second.second + adj_tip > ti_budget)
                throw std::runtime_error("Item budget exceeded!");
            magic_item_points_ += search->second.second;
            total_item_points_ += search->second.second;
            if (oco_extra_.first.find("Warpstone Tokens")) {
                magic_item_points_ -= oco_extra_.second.second;
                total_item_points_ -= oco_extra_.second.second;
            }
            // warptokens are arbitrarily duplicable across an army
            // so return an empty string to avoid doing limit checking
            removed.clear();
        }
    }
    points_ -= oco_extra_.second.second;
    oco_extra_.first = name;
    oco_extra_.second = search->second;
    points_ += search->second.second;
    return removed;
}

std::string character_unit::pick_mc_extra(const std::string& name) {
    auto search = handle_->opt().mc_extras().find(name);
    if (search == handle_->opt().mc_extras().end())
        throw std::invalid_argument("Item not found!");
    if (mc_extras_.count(name)) return std::string();
    mc_extras_[name] = search->second;
    points_ += search->second.second;
    return std::string();
}

std::string character_unit::remove_weapon(WeaponType wt, bool replacing) {
    if (!weapons_.count(wt)) return std::string();
    auto weapon = weapons_[wt];
    auto search = handle_->eq().weapons().find(wt);
    if (search != handle_->eq().weapons().cend()) {
        if (replacing) {
            weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        }
        else {
            if (search->second.second == std::get<1>(weapon)) return std::string();
            weapons_[wt] = {search->second.first, search->second.second, 0.0};
        }
    }
    else weapons_.erase(wt);
    // remove points value of weapon
    const double pts = std::get<2>(weapon);
    switch (std::get<0>(weapon)) {
    case ItemCategory::MUNDANE:
    case ItemCategory::NONE:
        break;
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    }
    points_ -= pts;
    return std::get<1>(weapon);
}

std::string character_unit::remove_armour(ArmourType at, bool replacing) {
    if (!armours_.count(at)) return std::string();
    auto armour = armours_[at];
    auto search = handle_->eq().armours().find(at);
    if (search != handle_->eq().armours().cend()) {
        if (replacing) {
            armours_.erase(at);
        } else {
            if (search->second.second == std::get<1>(armour)) return std::string();
            armours_[at] = {search->second.first, search->second.second, 0.0};
        }
    }
    else armours_.erase(at);
    // remove points value of armour
    const double pts = std::get<2>(armour);
    switch (std::get<0>(armour)) {
    case ItemCategory::MUNDANE:
    case ItemCategory::NONE:
        break;
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        break;
    }
    points_ -= pts;
    return std::get<1>(armour);
}

std::string character_unit::remove_oco_extra() {
    std::string removed = oco_extra_.first;
    points_ -= oco_extra_.second.second;
    oco_extra_.first = "";
    oco_extra_.second.second = 0.0;
    return removed;
}

std::string character_unit::remove_mc_extra(const std::string& name) {
    auto search = mc_extras_.find(name);
    if (search != mc_extras_.end()) {
        points_ -= search->second.second;
        mc_extras_.erase(name);
    }
    return name;
}

void character_unit::pick_mount(const std::string& name) {
    if (std::get<0>(mount_).name() == name) return;
    auto opt_search = handle_->opt().mounts().find(name);
    auto mnt_search = handle_->mounts_handle()->find(name);
    if (opt_search == handle_->opt().mounts().end() ||
            mnt_search == handle_->mounts_handle()->end())
        throw std::invalid_argument("Mount not found!");
    auto restr = restriction_check(RestrictionField::MOUNT, mnt_search->second.restrictions());
    if (restr.first) throw std::invalid_argument(restr.second);
    remove_mount();
    mount_ = {mnt_search->second, opt_search->second, {}, {}};
    points_ += opt_search->second;
}

void character_unit::remove_mount() {
    points_ -= std::get<1>(mount_);
    points_ -= std::get<2>(mount_).second;
    for (const auto& x : std::get<3>(mount_)) points_ -= x.second;
    std::get<0>(mount_) = mount();
    std::get<1>(mount_) = 0.0;
    std::get<2>(mount_).first.clear();
    std::get<2>(mount_).second = 0.0;
    std::get<3>(mount_).clear();
}

void character_unit::pick_mount_option(const std::string& name, bool oco) {
    std::unordered_map<std::string, double>::const_iterator search;
    if (oco) {
        search = std::get<0>(mount_).oco_extras().find(name);
        std::get<2>(mount_) = *search;
    } else {
        search = std::get<0>(mount_).mc_extras().find(name);
        std::get<3>(mount_).insert(*search);
    }
    points_ += search->second;
}

void character_unit::remove_mount_option(const std::string& name, bool oco) {
    if (oco) {
        if (std::get<2>(mount_).first != name)
            throw std::invalid_argument("Current mount does not have this option selected!");
        points_ -= std::get<2>(mount_).second;
        std::get<2>(mount_).first.clear();
        std::get<2>(mount_).second = 0.0;
    } else {
        if (!std::get<3>(mount_).count(name))
            throw std::invalid_argument("Current mount does not have this option selected!");
        points_ -= std::get<3>(mount_).at(name);
        std::get<3>(mount_).erase(name);
    }
}

std::string character_unit::pick_banner(ItemCategory item_class, const std::string& name) {
    if (!is_bsb())
        throw std::runtime_error("Only the Battle Standard Bearer may take a Magic Standard!");
    if (is_bsb() && (!talisman_.first.empty() || !enchanted_item_.first.empty() ||
                     !item_extras_.empty() ||
                     std::count_if(
                        std::begin(weapons_), std::end(weapons_),
                        [](const auto& w) {
                            return (std::get<0>(w.second) == ItemCategory::COMMON ||
                                    std::get<0>(w.second) == ItemCategory::MAGIC);
                        }
                     ) ||
                     std::count_if(
                        std::begin(armours_), std::end(armours_),
                        [](const auto& a) {
                            return (std::get<0>(a.second) == ItemCategory::COMMON ||
                                    std::get<0>(a.second) == ItemCategory::MAGIC);
                        }
                     ))
       ) throw std::runtime_error("BSBs equipped with Magic Equipment cannot take Magic Standards!");
    std::string removed;
    switch (item_class) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        removed = pick_magic_item(ItemType::BANNER, item_class, name);
        break;
    default: break;
    }
    return removed;
}

std::string character_unit::remove_banner() {
    std::string removed = banner.first;
    points_ -= banner.second.second;
    switch (banner.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= banner.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= banner.second.second;
        break;
    default: break;
    }
    total_item_points_ -= banner.second.second;
    banner.first.clear();
    banner.second.second = 0.0;
    return removed;
}

std::string character_unit::html_table_row_both(short mlevel, std::string arcane) const {
    std::string row = "<tr>";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit mount
    row += "<td>" + (std::get<0>(mount_).name().empty() ? "&nbsp;" : std::get<0>(mount_).name()) + "</td>\n";
    // unit mage level
    if (mlevel == -1) row += "<td>None</td>\n";
    else row += "<td>Level " + std::to_string(mlevel) + "</td>\n";
    // weapons
    if (weapons_.count(WeaponType::MELEE))
        row += "<td><strong>Melee:</strong> " +
                std::get<1>(weapons_.at(WeaponType::MELEE)) +
                (weapons_.count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (weapons_.count(WeaponType::BALLISTIC))
        row +=  std::string(weapons_.count(WeaponType::MELEE) ? "" : "<td>") + "<strong>Ranged:</strong> " +
                std::get<1>(weapons_.at(WeaponType::BALLISTIC)) + "</td>\n";
    // armour
    if (armours_.count(ArmourType::ARMOUR))
        row += "<td><strong>Body:</strong> " +
                std::get<1>(armours_.at(ArmourType::ARMOUR)) +
                (armours_.count(ArmourType::SHIELD) || armours_.count(ArmourType::HELMET)
                    ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::SHIELD))
        row +=  std::string(armours_.count(ArmourType::ARMOUR) ? "" : "<td>") + "<strong>Shield:</strong> " +
                std::get<1>(armours_.at(ArmourType::SHIELD)) +
                (armours_.count(ArmourType::HELMET) ? "<br/>" : "</td>\n");
    if (armours_.count(ArmourType::HELMET))
        row +=  std::string((armours_.count(ArmourType::ARMOUR) || armours_.count(ArmourType::SHIELD))
                            ? "" : "<td>") + "<strong>Helmet:</strong> " +
                std::get<1>(armours_.at(ArmourType::HELMET)) + "</td>\n";
    if (!armours_.count(ArmourType::ARMOUR) &&
            !armours_.count(ArmourType::SHIELD) &&
            !armours_.count(ArmourType::HELMET)) row += "<td>&nbsp;</td>";
    // talisman
    row += "<td>";
    if (!talisman_.first.empty()) row += talisman_.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // enchanted item
    row += "<td>";
    if (!enchanted_item_.first.empty()) row += enchanted_item_.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // arcane item
    if (arcane.empty()) row += "<td>&nbsp;</td>\n";
    else row += "<td>" + arcane + "</td>\n";
    // magic/faction item extras
    row += "<td>";
    for (const auto& x : item_extras_) row += x.first + "<br/>";
    if (item_extras_.empty()) row += "&nbsp;";
    row += "</td>\n";
    // other extras
    row += "<td>";
    if (!oco_extra_.first.empty()) row += oco_extra_.first + (mc_extras().empty() ? "" : "<br/>");
    for (const auto& x : mc_extras_) row += x.first + "<br/>";
    if (oco_extra_.first.empty() && mc_extras_.empty()) row += "&nbsp;";
    row += "</td>\n";
    // banner (if BSB)
    row += "<td>";
    if (!banner.first.empty()) row += banner.first;
    else row += "&nbsp;";
    row += "</td>\n";
    // characteristics
    row += "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    row += "<thead><tr>\n"
            "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
            "<th>I</th><th>A</th><th>Ld</th>\n"
           "</tr></thead>\n";
    row += "<tr>\n";
    for (const auto& x : handle_->statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    if (!std::get<0>(mount_).name().empty()) {
        row += "<tr>\n";
        for (const auto& x : std::get<0>(mount_).statistics()) row += "<td align=\"center\">" + x + "</td>\n";
        row += "</tr>\n";
    }
    row += "</table></td>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}

std::string character_unit::html_table_row() const {
    return html_table_row_both(-1, std::string());
}

std::string character_unit::save() const {
    return "";
}
