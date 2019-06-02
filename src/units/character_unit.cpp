#include "character_unit.h"
#include "../army_list.h"

character_unit::character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : unit(base, army_handle),
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
}

character_unit::character_unit(const character_unit& other)
 : unit(other), weapons_(other.weapons_), armours_(other.armours_),
   talisman_(other.talisman_), enchanted_item_(other.enchanted_item_),
   arcane_item_(other.arcane_item_), oco_extra_(other.oco_extra_),
   mc_extras_(other.mc_extras_), item_extras_(other.item_extras_), mount_(other.mount_),
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

std::string character_unit::restriction_check(
    const std::unordered_multimap<RestrictionField, std::any>& restrictions,
    const std::string& item_name
) const {
    std::string msg = unit::restriction_check(restrictions, item_name);
    for (const auto& restriction : restrictions) {
        switch (restriction.first) {
        case RestrictionField::TALISMAN:
        case RestrictionField::ENCHANTED:
        case RestrictionField::OTHER:
            for (const auto& x : std::any_cast<std::vector<std::string>>(restriction.second)) {
                bool restricted = false;
                switch (restriction.first) {
                case RestrictionField::TALISMAN:
                    restricted = talisman().first != x;
                    break;
                case RestrictionField::ENCHANTED:
                    restricted = enchanted_item().first != x;
                    break;
                case RestrictionField::OTHER:
                    restricted = !(item_extras().count(x));
                    break;
                default: break;
                }
                if (restricted) {
                    if (msg.empty()) msg += name() + " requires: " + x;
                    else msg += ", " + x;
                }
            }
            break;
        default: break;
        }
    }
    return msg;
}

std::size_t character_unit::size() const noexcept { return 1U; }

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::weapons() const noexcept { return weapons_; }

std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::weapons_access() noexcept { return weapons_; }

const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::armour() const noexcept { return armours_; }

std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& character_unit::armour_access() noexcept { return armours_; }

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::talisman() const noexcept {
    return talisman_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::talisman_access() noexcept { return talisman_; }

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::enchanted_item() const noexcept {
    return enchanted_item_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::enchanted_item_access() noexcept { return enchanted_item_; }

const std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::arcane_item() const noexcept {
    return arcane_item_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::arcane_item_access() noexcept { return arcane_item_; }

const std::unordered_map<std::string, std::pair<ItemCategory, double>>& character_unit::item_extras() const noexcept {
    return item_extras_;
}

std::unordered_map<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::item_extras_access() noexcept { return item_extras_; }

const std::pair<std::string, std::pair<bool, double>>& character_unit::oco_extra() const noexcept {
    return oco_extra_;
}
const std::unordered_map<std::string, std::pair<bool, double>>& character_unit::mc_extras() const noexcept {
    return mc_extras_;
}

const std::tuple<
    std::string,
    double,
    std::pair<std::string, double>,
    std::unordered_map<std::string, double>
>& character_unit::mnt() const noexcept { return mount_; }

const std::pair<std::string, std::pair<ItemCategory, double>>& character_unit::magic_banner() const noexcept {
    return banner;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& character_unit::magic_banner_acces() noexcept { return banner; }

std::string character_unit::pick_weapon(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle_->opt().weapons().find(name);
        if (search == handle_->opt().weapons().cend())
            throw std::invalid_argument("Weapon not found!");
        removed = remove_weapon(search->second.type);
        do_replacements(search->second.replacements);
        weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        try { removed = pick_magic_item(ItemType::WEAPON, item_category, name); }
        catch (const std::exception&) { throw; }
        break;
    default: break;
    }
    return removed;
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
        --n_magic_items;
        army_->decr_item_tracker(std::get<1>(weapon));
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        --n_magic_items;
        army_->decr_item_tracker(std::get<1>(weapon));
        break;
    }
    points_ -= pts;
    return std::get<1>(weapon);
}

std::string character_unit::pick_armour(ItemCategory item_category, const std::string& name) {
    std::string removed;
    switch (item_category) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle_->opt().armour().find(name);
        if (search == handle_->opt().armour().cend())
            throw std::invalid_argument("Armour not found!");
        removed = remove_armour(search->second.type);
        do_replacements(search->second.replacements);
        armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
    case ItemCategory::FACTION:
        try { removed = pick_magic_item(ItemType::ARMOUR, item_category, name); }
        catch (const std::exception&) { throw; }
        break;
    default: break;
    }
    return removed;
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
        --n_magic_items;
        army_->decr_item_tracker(std::get<1>(armour));
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= pts;
        total_item_points_ -= pts;
        --n_magic_items;
        army_->decr_item_tracker(std::get<1>(armour));
        break;
    }
    points_ -= pts;
    return std::get<1>(armour);
}

std::string character_unit::pick_talisman(ItemCategory item_category, const std::string& name) {
    std::string removed;
    try { removed = pick_magic_item(ItemType::TALISMAN, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}


std::string character_unit::pick_enchanted_item(ItemCategory item_category, const std::string& name) {
    std::string removed;
    try { removed = pick_magic_item(ItemType::ENCHANTED, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}

std::string character_unit::pick_arcane_item(ItemCategory item_category, const std::string& name) {
    std::string removed;
    if (!is_mage()) return removed;
    try { removed = pick_magic_item(ItemType::ARCANE, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}

std::string character_unit::pick_magic_extra(ItemCategory item_category, const std::string& name) {
    std::string removed;
    try { removed = pick_magic_item(ItemType::OTHER, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}

std::string character_unit::pick_oco_extra(const std::string& name) {
    std::string removed;
    auto search = handle_->opt().oco_extras().find(name);
    if (search == handle_->opt().oco_extras().end())
        throw std::invalid_argument("Item not found!");
    if (oco_extra_.first == name) return removed;
    removed = oco_extra_.first;
    // we need to handle warpstone tokens specifically as they count as arcane
    // items however can be bought in conjunction with other arcane items
    if (handle_->faction() == Faction::SKAVEN) {
        if (name.find("Warpstone Tokens")) {
            const double mi_budget = handle_->magic_item_budget().points;
            const double ti_budget = handle_->total_item_budget().points;
            double adj_mip = magic_item_points_;
            double adj_tip = total_item_points_;
            if (oco_extra_.first.find("Warpstone Tokens")) {
                adj_mip -= oco_extra_.second.second;
                adj_tip -= oco_extra_.second.second;
            }
            if (search->second.points + adj_mip > mi_budget ||
                    search->second.points + adj_tip > ti_budget)
                throw std::runtime_error("Item budget exceeded!");
            magic_item_points_ += search->second.points;
            total_item_points_ += search->second.points;
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
    oco_extra_.second = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return removed;
}

std::string character_unit::remove_oco_extra() {
    std::string removed = oco_extra_.first;
    if (handle_->faction() == Faction::SKAVEN) {
        if (removed.find("Warpstone Tokens")) {
            magic_item_points_ -= oco_extra_.second.second;
            total_item_points_ -= oco_extra_.second.second;
        }
    }
    points_ -= oco_extra_.second.second;
    oco_extra_.first = "";
    oco_extra_.second.second = 0.0;
    return removed;
}

std::string character_unit::pick_mc_extra(const std::string& name) {
    auto search = handle_->opt().mc_extras().find(name);
    if (search == handle_->opt().mc_extras().end())
        throw std::invalid_argument("Item not found!");
    auto restr = restriction_check(search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    if (mc_extras_.count(name)) return std::string();
    mc_extras_[name] = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return std::string();
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
    if (std::get<0>(mount_) == name) return;
    auto opt_search = handle_->opt().mounts().find(name);
    if (opt_search == std::end(handle_->opt().mounts()))
        throw std::invalid_argument("Mount: " + name + " not found!");
    auto restr = restriction_check(opt_search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    remove_mount();
    mount_ = {name, opt_search->second.points, {}, {}};
    points_ += opt_search->second.points;
}

void character_unit::remove_mount() {
    points_ -= std::get<1>(mount_);
    points_ -= std::get<2>(mount_).second;
    for (const auto& x : std::get<3>(mount_)) points_ -= x.second;
    std::get<0>(mount_).clear();
    std::get<1>(mount_) = 0.0;
    // clear mount oco extra
    std::get<2>(mount_).first.clear();
    std::get<2>(mount_).second = 0.0;
    // clear mount mc extras
    std::get<3>(mount_).clear();
}

void character_unit::pick_mount_option(const std::string& name, bool oco) {
    std::unordered_map<std::string, extra_option>::const_iterator search;
    if (oco) {
        search = handle_->opt().mounts().find(std::get<0>(mount_))->second.oco_extras.find(name);
        auto tmp = search->second.restrictions;
        auto restr = restriction_check(search->second.restrictions, name);
        if (!restr.empty()) throw std::invalid_argument(restr);
        std::get<2>(mount_) = {search->second.name, search->second.points};
    } else {
        search = handle_->opt().mounts().find(std::get<0>(mount_))->second.mc_extras.find(name);
        auto restr = restriction_check(search->second.restrictions, name);
        if (!restr.empty()) throw std::invalid_argument(restr);
        std::get<3>(mount_).insert({search->second.name, search->second.points});
    }
    points_ += search->second.points;
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

std::string character_unit::pick_banner(ItemCategory item_category, const std::string& name) {
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
    try { removed = pick_magic_item(ItemType::BANNER, item_category, name); }
    catch (const std::exception&) { throw; }
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
    army_->decr_item_tracker(removed);
    return removed;
}

std::vector<std::string> character_unit::clear() {
    auto removed = unit::clear();
    // remove talisman
    auto talisman_rmvd = remove_talisman();
    if (!talisman_rmvd.empty()) removed.push_back(talisman_rmvd);
    // remove enchanted item
    auto ei_rmvd = remove_enchanted_item();
    if (!ei_rmvd.empty()) removed.push_back(ei_rmvd);
    // remove others
    if (!item_extras().empty()) {
        std::vector<std::string> other_names(item_extras().size());
        std::generate(
            std::begin(other_names), std::end(other_names),
            [it=std::cbegin(item_extras())]() mutable { return (it++)->first; }
        );
        for (const auto& name : other_names) {
            auto other_rmvd = remove_magic_extra(name);
            if (!other_rmvd.empty()) removed.push_back(other_rmvd);
        }
    }
    return removed;
}

std::string character_unit::save() const {
    return "";
}
