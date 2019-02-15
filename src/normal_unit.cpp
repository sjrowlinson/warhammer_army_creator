#include "normal_unit.h"
#include "army_list.h"

normal_unit::normal_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : unit(base, army_handle),
      handle(std::dynamic_pointer_cast<base_normal_unit>(base)) {
    points_ = handle->min_size() * handle->points_per_model();
    model_select_ = ModelSelect::DEFAULT;
    size_ = handle->min_size();
    for (const auto& x : handle->eq().weapons()) {
        auto p = x.second;
        weapons_[x.first] = {p.first, p.second, 0.0};
    }
    for (const auto& x : handle->eq().armours()) {
        auto p = x.second;
        armours_[x.first] = {p.first, p.second, 0.0};
    }
}

normal_unit::normal_unit(const normal_unit& other)
    : unit(other),
      size_(other.size_), weapons_(other.weapons_),
      armours_(other.armours_), oco_extra_(other.oco_extra_),
      mc_extras_(other.mc_extras_), champ_weapons_(other.champ_weapons_),
      champ_armours_(other.champ_armours_),
      champ_talisman_(other.champ_talisman_),
      champ_enchanted_item_(other.champ_enchanted_item_),
      champ_arcane_(other.champ_arcane_),
      champ_item_extras_(other.champ_item_extras_),
      champ_oco_extra_(other.champ_oco_extra_),
      champ_mc_extras_(other.champ_mc_extras_),
      command_group(other.command_group),
      banner(other.banner), handle(other.handle) {}

// current property accessors

bool normal_unit::is_character() const noexcept { return false; }
bool normal_unit::is_mage() const noexcept { return false; }
bool normal_unit::is_mixed() const noexcept { return false; }

bool normal_unit::switch_model_select(ModelSelect ms) {
    model_select_ = ms;
    return true;
}

std::size_t normal_unit::size() const noexcept { return size_; }

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::weapons() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return weapons_;
    case ModelSelect::CHAMPION:
        return champ_weapons_;
    }
    return weapons_;
}

std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::weapons_access() noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return weapons_;
    case ModelSelect::CHAMPION:
        return champ_weapons_;
    }
    return weapons_;
}

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::champion_weapons() const noexcept {
    return champ_weapons_;
}

const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::armour() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return armours_;
    case ModelSelect::CHAMPION:
        return champ_armours_;
    }
    return armours_;
}

std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::armour_access() noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return armours_;
    case ModelSelect::CHAMPION:
        return champ_armours_;
    }
    return armours_;
}

const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& normal_unit::champion_armour() const noexcept {
    return champ_armours_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::talisman_access() noexcept {
    return champ_talisman_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::enchanted_item_access() noexcept {
    return champ_enchanted_item_;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::arcane_item_access() noexcept {
    return champ_arcane_;
}

std::unordered_map<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::item_extras_access() noexcept {
    return champ_item_extras_;
}

const std::pair<
    std::string,
    std::pair<bool, double>
>& normal_unit::oco_extra() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return oco_extra_;
    case ModelSelect::CHAMPION:
        return champ_oco_extra_;
    }
    return oco_extra_;
 }

const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& normal_unit::mc_extras() const noexcept {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return mc_extras_;
    case ModelSelect::CHAMPION:
        return champ_mc_extras_;
    }
    return mc_extras_;
 }

const std::pair<
    std::string,
    std::pair<bool, double>
>& normal_unit::champion_oco_extra() const noexcept { return champ_oco_extra_; }

const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& normal_unit::champion_mc_extras() const noexcept { return champ_mc_extras_; }

const std::tuple<
    std::string,
    double,
    std::pair<std::string, double>,
    std::unordered_map<std::string, double>
>& normal_unit::mnt() const noexcept { return mount_; }

const std::unordered_map<
    CommandGroup, std::pair<std::string, double>
>& normal_unit::command() const noexcept {
    return command_group;
}

const std::pair<std::string, std::pair<ItemCategory, double>>& normal_unit::talisman() const noexcept {
    return champ_talisman_;
}

const std::pair<std::string, std::pair<ItemCategory, double>>& normal_unit::enchanted_item() const noexcept {
    return champ_enchanted_item_;
}

const std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::arcane_item() const noexcept {
    return champ_arcane_;
}

const std::unordered_map<std::string, std::pair<ItemCategory, double>>& normal_unit::item_extras() const noexcept {
    return champ_item_extras_;
}

const std::pair<std::string, std::pair<ItemCategory, double>>& normal_unit::magic_banner() const noexcept {
    return banner;
}

std::pair<
    std::string,
    std::pair<ItemCategory, double>
>& normal_unit::magic_banner_acces() noexcept {
    return banner;
}

// current property modifiers

std::string normal_unit::pick_weapon(ItemCategory item_type, const std::string& name) {
    switch(model_select_) {
    case ModelSelect::DEFAULT:
        return pick_default_weapon(item_type, name);
    case ModelSelect::CHAMPION:
        return pick_champion_weapon(item_type, name);
    }
    return pick_default_weapon(item_type, name);
}

std::string normal_unit::remove_weapon(WeaponType wt, bool replacing) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return remove_default_weapon(wt, replacing);
    case ModelSelect::CHAMPION:
        return remove_champion_weapon(wt, replacing);
    }
    return remove_default_weapon(wt, replacing);
}

std::string normal_unit::pick_default_weapon(ItemCategory item_type, const std::string& name) {
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->opt().weapons().find(name);
        if (search == handle->opt().weapons().cend()) {
            throw std::invalid_argument("Weapon not found!");
        }
        removed = remove_weapon(search->second.type);
        do_replacements(search->second.replacements);
        weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += size_ * search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        throw std::invalid_argument("Cannot give magic weapons to this unit!");
    case ItemCategory::FACTION:
        throw std::invalid_argument("Cannot give faction weapons to this unit!");
    default: break;
    }
    return removed;
}

std::string normal_unit::remove_default_weapon(WeaponType wt, bool replacing) {
    if (!weapons_.count(wt)) return std::string();
    auto weapon = weapons_[wt];
    auto search = handle->eq().weapons().find(wt);
    if (search != handle->eq().weapons().cend()) {
        if (replacing) {
            weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else {
            if (search->second.second == std::get<1>(weapon)) return std::string();
            weapons_[wt] = {search->second.first, search->second.second, 0.0};
        }
    }
    else weapons_.erase(wt);
    points_ -= size_ * std::get<2>(weapon);
    return std::get<1>(weapon);
}

std::string normal_unit::pick_champion_weapon(ItemCategory item_type, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->champion_opt().weapons().find(name);
        if (search == handle->champion_opt().weapons().cend())
            throw std::invalid_argument("Weapon not found!");
        removed = remove_champion_weapon(search->second.type);
        do_replacements(search->second.replacements, true);
        champ_weapons_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::FACTION:
    case ItemCategory::COMMON:
        switch_model_select(ModelSelect::CHAMPION);
        try { removed = pick_magic_item(ItemType::WEAPON, item_type, name); }
        catch (const std::exception&) {
            switch_model_select(ModelSelect::DEFAULT);
            throw;
        }
        switch_model_select(ModelSelect::DEFAULT);
        break;
    default: break;
    }
    return removed;
}

std::string normal_unit::remove_champion_weapon(WeaponType wt, bool replacing) {
    if (!champ_weapons_.count(wt)) return std::string();
    auto weapon = champ_weapons_[wt];
    auto def_w = handle->champion_eq().weapons().find(wt);
    if (def_w != handle->champion_eq().weapons().cend()) {
        if (replacing) {
            champ_weapons_.erase(wt);
            if (wt == WeaponType::MELEE)
                champ_weapons_[wt] = {ItemCategory::MUNDANE, "Hand weapon", 0.0};
        } else {
            if (def_w->second.second == std::get<1>(weapon)) return std::string();
            champ_weapons_[wt] = {def_w->second.first, def_w->second.second, 0.0};
        }
    }
    else champ_weapons_.erase(wt);
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

std::string normal_unit::pick_armour(ItemCategory item_type, const std::string& name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return pick_default_armour(item_type, name);
    case ModelSelect::CHAMPION:
        return pick_champion_armour(item_type, name);
    }
    return pick_default_armour(item_type, name);
}

std::string normal_unit::remove_armour(ArmourType at, bool replacing) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return remove_default_armour(at, replacing);
    case ModelSelect::CHAMPION:
        return remove_champion_armour(at, replacing);
    }
    return remove_default_armour(at, replacing);
}

std::string normal_unit::pick_default_armour(ItemCategory item_type, const std::string& name) {
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->opt().armour().find(name);
        if (search == handle->opt().armour().end())
            throw std::invalid_argument("Armour not found!");
        removed = remove_armour(search->second.type);
        do_replacements(search->second.replacements);
        armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += size_ * search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        throw std::invalid_argument("Cannot give magic armour to this unit!");
    case ItemCategory::FACTION:
        throw std::invalid_argument("Cannot give faction armour to this unit!");
    default: break;
    }
    return removed;
}

std::string normal_unit::remove_default_armour(ArmourType at, bool replacing) {
    if (!armours_.count(at)) return std::string();
    auto armour = armours_[at];
    auto search = handle->eq().armours().find(at);
    if (search != handle->eq().armours().cend()) {
        if (replacing) armours_.erase(at);
        else {
            if (search->second.second == std::get<1>(armour)) return std::string();
            armours_[at] = {search->second.first, search->second.second, 0.0};
        }
    }
    else armours_.erase(at);
    points_ -= size_ * std::get<2>(armour);
    return std::get<1>(armour);
}

std::string normal_unit::pick_champion_armour(ItemCategory item_type, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch (item_type) {
    case ItemCategory::MUNDANE:
    {
        auto search = handle->champion_opt().armour().find(name);
        if (search == handle->champion_opt().armour().end())
            throw std::invalid_argument("Armour not found!");
        removed = remove_champion_armour(search->second.type);
        do_replacements(search->second.replacements, true);
        champ_armours_[search->second.type] = {search->second.category, search->first, search->second.points};
        points_ += search->second.points;
        break;
    }
    case ItemCategory::MAGIC:
    case ItemCategory::FACTION:
    case ItemCategory::COMMON:
        switch_model_select(ModelSelect::CHAMPION);
        try { removed = pick_magic_item(ItemType::ARMOUR, item_type, name); }
        catch (const std::exception&) {
            switch_model_select(ModelSelect::DEFAULT);
            throw;
        }
        switch_model_select(ModelSelect::DEFAULT);
        break;
    default: break;
    }
    return removed;
}

std::string normal_unit::remove_champion_armour(ArmourType at, bool replacing) {
    if (!champ_armours_.count(at)) return std::string();
    auto armour = champ_armours_[at];
    auto def_a = handle->champion_eq().armours().find(at);
    if (def_a != handle->champion_eq().armours().cend()) {
        if (replacing) champ_armours_.erase(at);
        else {
            if (def_a->second.second == std::get<1>(armour)) return std::string();
            champ_armours_[at] = {def_a->second.first, def_a->second.second, 0.0};
        }
    }
    else champ_armours_.erase(at);
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

std::string normal_unit::pick_talisman(ItemCategory item_category, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::TALISMAN, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}

// TODO: could just define remove_talisman and a few others (e.g. enchanted_item) in
//       unit and use talisman_access() in method body
std::string normal_unit::remove_talisman() {
    std::string removed = champ_talisman_.first;
    points_ -= champ_talisman_.second.second;
    switch (champ_talisman_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= champ_talisman_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= champ_talisman_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= champ_talisman_.second.second;
    champ_talisman_.first.clear();
    champ_talisman_.second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(removed);
    return removed;
}

std::string normal_unit::pick_enchanted_item(ItemCategory item_category, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::ENCHANTED, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}

std::string normal_unit::remove_enchanted_item() {
    std::string removed = champ_enchanted_item_.first;
    points_ -= champ_enchanted_item_.second.second;
    switch (champ_enchanted_item_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= champ_enchanted_item_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= champ_enchanted_item_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= champ_enchanted_item_.second.second;
    champ_enchanted_item_.first.clear();
    champ_enchanted_item_.second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(removed);
    return removed;
}

std::string normal_unit::pick_arcane_item(ItemCategory item_category, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::ARCANE, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}

std::string normal_unit::remove_arcane_item() {
    std::string removed = champ_arcane_.first;
    points_ -= champ_arcane_.second.second;
    switch (champ_arcane_.second.first) {
    case ItemCategory::MAGIC:
    case ItemCategory::COMMON:
        magic_item_points_ -= champ_arcane_.second.second;
        break;
    case ItemCategory::FACTION:
        faction_item_points_ -= champ_arcane_.second.second;
        break;
    default: break;
    }
    total_item_points_ -= champ_arcane_.second.second;
    champ_arcane_.first.clear();
    champ_arcane_.second.second = 0.0;
    --n_magic_items;
    army_->decr_item_tracker(removed);
    return removed;
}

std::string normal_unit::pick_magic_extra(ItemCategory item_category, const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    std::string removed;
    switch_model_select(ModelSelect::CHAMPION);
    try { removed = pick_magic_item(ItemType::OTHER, item_category, name); }
    catch (const std::exception&) {
        switch_model_select(ModelSelect::DEFAULT);
        throw;
    }
    switch_model_select(ModelSelect::DEFAULT);
    return removed;
}

std::string normal_unit::remove_magic_extra(const std::string& name) {
    auto search = champ_item_extras_.find(name);
    if (search == champ_item_extras_.end())
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
    champ_item_extras_.erase(name);
    --n_magic_items;
    army_->decr_item_tracker(name);
    return name;
}

std::string normal_unit::pick_banner(ItemCategory item_category, const std::string& name) {
    if (!(command_group.count(CommandGroup::STANDARD_BEARER)))
        throw std::runtime_error("Unit contains no standard bearer!");
    std::string removed;
    try { removed = pick_magic_item(ItemType::BANNER, item_category, name); }
    catch (const std::exception&) { throw; }
    return removed;
}

std::string normal_unit::remove_banner() {
    std::string removed = banner.first;
    if (banner.first.empty()) return std::string();
    points_ -= banner.second.second;
    banner.first.clear();
    banner.second.second = 0.0;
    army_->decr_item_tracker(removed);
    return removed;
}

std::string normal_unit::pick_oco_extra(const std::string& name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return pick_default_oco_extra(name);
    case ModelSelect::CHAMPION:
        return pick_champion_oco_extra(name);
    }
    return pick_default_oco_extra(name);
}

std::string normal_unit::remove_oco_extra() {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return remove_default_oco_extra();
    case ModelSelect::CHAMPION:
        return remove_champion_oco_extra();
    }
    return remove_default_oco_extra();
}

std::string normal_unit::pick_default_oco_extra(const std::string& name) {
    auto search = handle->opt().oco_extras().find(name);
    if (search == handle->opt().oco_extras().end())
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (oco_extra_.first == name) return std::string();
    removed = oco_extra_.first;
    const double prev_pts = oco_extra_.second.second;
    points_ -= (oco_extra_.second.first) ? prev_pts : size_ * prev_pts;
    oco_extra_.first = name;
    oco_extra_.second = {search->second.is_singular, search->second.points};
    const double new_pts = search->second.points;
    points_ += (search->second.is_singular) ? new_pts : size_ * new_pts;
    return removed;
}

std::string normal_unit::remove_default_oco_extra() {
    std::string removed = oco_extra_.first;
    points_ -= (oco_extra_.second.first) ? oco_extra_.second.second : size_ * oco_extra_.second.second;
    oco_extra_.first.clear();
    oco_extra_.second.second = 0.0;
    return removed;
}

std::string normal_unit::pick_champion_oco_extra(const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->champion_opt().oco_extras().find(name);
    if (search == handle->champion_opt().oco_extras().end())
        throw std::invalid_argument("Item not found!");
    std::string removed;
    if (champ_oco_extra_.first == name) return std::string();
    removed = champ_oco_extra_.first;
    points_ -= champ_oco_extra_.second.second;
    champ_oco_extra_.first = name;
    champ_oco_extra_.second = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return removed;
}

std::string normal_unit::remove_champion_oco_extra() {
    std::string removed = champ_oco_extra_.first;
    points_ -= champ_oco_extra_.second.second;
    champ_oco_extra_.first = "";
    champ_oco_extra_.second = {true, 0.0};
    return removed;
}

std::string normal_unit::pick_mc_extra(const std::string& name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return pick_default_mc_extra(name);
    case ModelSelect::CHAMPION:
        return pick_champion_mc_extra(name);
    }
    return pick_default_mc_extra(name);
}

std::string normal_unit::remove_mc_extra(const std::string& name) {
    switch (model_select_) {
    case ModelSelect::DEFAULT:
        return remove_default_mc_extra(name);
    case ModelSelect::CHAMPION:
        return remove_champion_mc_extra(name);
    }
    return remove_default_mc_extra(name);
}

std::string normal_unit::pick_default_mc_extra(const std::string& name) {
    auto search = handle->opt().mc_extras().find(name);
    if (search == handle->opt().mc_extras().end())
        throw std::invalid_argument("Item not found!");
    auto restr = restriction_check(search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    if (mc_extras_.count(name)) return std::string();
    mc_extras_[name] = {search->second.is_singular, search->second.points};
    const double pts = search->second.points;
    points_ += (search->second.is_singular) ? pts : size_ * pts;
    return std::string();
}

std::string normal_unit::remove_default_mc_extra(const std::string& name) {
    auto search = mc_extras_.find(name);
    if (search != mc_extras_.end()) {
        const double pts = search->second.second;
        points_ -= (search->second.first) ? pts : size_ * pts;
        mc_extras_.erase(name);
    }
    return std::string();
}

std::string normal_unit::pick_champion_mc_extra(const std::string& name) {
    if (!(command_group.count(CommandGroup::CHAMPION)))
        throw std::runtime_error("No champion present in the unit!");
    auto search = handle->champion_opt().mc_extras().find(name);
    if (search == handle->champion_opt().mc_extras().end())
        throw std::invalid_argument("Item not found!");
    if (champ_mc_extras_.count(name)) return std::string();
    champ_mc_extras_[name] = {search->second.is_singular, search->second.points};
    points_ += search->second.points;
    return std::string();
}

std::string normal_unit::remove_champion_mc_extra(const std::string& name) {
    auto search = champ_mc_extras_.find(name);
    if (search != champ_mc_extras_.end()) {
        points_ -= search->second.second;
        champ_mc_extras_.erase(name);
    }
    return std::string();
}

void normal_unit::pick_mount(const std::string& name) {
    if (std::get<0>(mount_) == name) return;
    auto opt_search = handle->opt().mounts().find(name);
    if (opt_search == std::end(handle->opt().mounts()))
        throw std::invalid_argument("Mount: " + name + " not found!");
    auto restr = restriction_check(opt_search->second.restrictions, name);
    if (!restr.empty()) throw std::invalid_argument(restr);
    remove_mount();
    mount_ = {name, opt_search->second.points, {}, {}};
    points_ += opt_search->second.points;
}

void normal_unit::remove_mount() {
    points_ -= std::get<1>(mount_);
    points_ -= std::get<2>(mount_).second;
    for (const auto& x : std::get<3>(mount_)) points_ -= x.second;
    std::get<0>(mount_).clear();
    std::get<1>(mount_) = 0.0;
    std::get<2>(mount_).first.clear();
    std::get<2>(mount_).second = 0.0;
    std::get<3>(mount_).clear();
}

void normal_unit::add_command_member(CommandGroup member) {
    auto search = handle->optional_command().find(member);
    if (search == handle->optional_command().end())
        throw std::invalid_argument("Command group member not available!");
    command_group[member] = {search->second.first, search->second.second};
    points_ += search->second.second;
}

void normal_unit::remove_command_member(CommandGroup member) {
    auto it = command_group.find(member);
    if (it != command_group.end()) {
        points_ -= it->second.second;
        if (it->first == CommandGroup::CHAMPION) {
            remove_champion_armour(ArmourType::ARMOUR);
            remove_champion_armour(ArmourType::SHIELD);
            remove_champion_armour(ArmourType::HELMET);
            remove_champion_weapon(WeaponType::MELEE);
            remove_champion_weapon(WeaponType::BALLISTIC);
            remove_champion_oco_extra();
            std::vector<std::string> champ_extra_names;
            for (auto extra : champ_mc_extras_) champ_extra_names.push_back(extra.first);
            for (auto e : champ_extra_names) remove_champion_mc_extra(e);
            //for (auto extra : champ_mc_extras_)
            //    remove_champion_mc_extra(extra.first);
        }
        else if (it->first == CommandGroup::STANDARD_BEARER) {
            remove_banner();
        }
        command_group.erase(member);
    }
}

void normal_unit::change_size(std::size_t n) {
    if (n < handle->min_size() || n > handle->max_size())
        throw std::invalid_argument("Invalid unit size!");
    points_ = n * handle->points_per_model();
    // add back default unit item points
    for (const auto& w : weapons_) points_ += n * std::get<2>(w.second);
    for (const auto& a : armours_) points_ += n * std::get<2>(a.second);
    points_ += (oco_extra_.second.first) ? oco_extra_.second.second : n * oco_extra_.second.second;
    for (const auto& e : mc_extras_)
        points_ += (e.second.first) ? e.second.second : n * e.second.second;
    // add back champion item points
    for (const auto& cw : champ_weapons_) points_ += std::get<2>(cw.second);
    for (const auto& ca : champ_armours_) points_ += std::get<2>(ca.second);
    points_ += champ_talisman_.second.second;
    points_ += champ_enchanted_item_.second.second;
    points_ += champ_arcane_.second.second;
    for (const auto& ce : champ_item_extras_)
        points_ += ce.second.second;
    points_ += champ_oco_extra_.second.second;
    for (const auto& ce : champ_mc_extras_) points_ += ce.second.second;
    points_ += n * std::get<1>(mount_);
    // add back command members and banner points
    for (const auto& m : command_group) points_ += m.second.second;
    points_ += banner.second.second;
    size_ = n;
}

std::string normal_unit::html_table_row() const {
    std::string colour;
    switch (handle->unit_type()) {
    case UnitType::CORE:
        colour = "#D4EFDF";
        break;
    case UnitType::SPECIAL:
        colour = "#D6EAF8";
        break;
    case UnitType::RARE:
        colour = "#F2D7D5";
        break;
    default: break;
    }
    std::string row = "<tr style=\"background-color:" + colour + "\">\n";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit size
    row += "<td>" + std::to_string(size_) + "</td>\n";
    // unit mount
    row += "<td>" + (std::get<0>(mount_).empty() ? "&nbsp;" : std::get<0>(mount_)) + "</td>\n";
    // weapons
    if (weapons_.empty()) row += "<td>&nbsp;</td>\n";
    if (weapons_.count(WeaponType::MELEE))
        row += "<td><strong>Melee:</strong> " +
                std::get<1>(weapons_.at(WeaponType::MELEE)) +
                (weapons_.count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (weapons_.count(WeaponType::BALLISTIC))
        row +=  std::string(weapons_.count(WeaponType::MELEE) ? "" : "<td>") + "<strong>Ranged:</strong> " +
                std::get<1>(weapons_.at(WeaponType::BALLISTIC)) + "</td>\n";
    // armour
    if (armours_.empty()) row += "<td>&nbsp;</td>\n";
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
    // extras
    row += "<td>";
    if (!oco_extra_.first.empty()) row += oco_extra_.first + (mc_extras().empty() ? "" : "<br/>");
    for (const auto& x : mc_extras_) row += x.first + "<br/>";
    if (oco_extra_.first.empty() && mc_extras_.empty()) row += "&nbsp;";
    row += "</td>\n";
    // command
    if (command_group.count(CommandGroup::MUSICIAN))
        row += "<td>" +
                command_group.at(CommandGroup::MUSICIAN).first +
                (command_group.count(CommandGroup::STANDARD_BEARER) || command_group.count(CommandGroup::CHAMPION)
                    ? "<br/>" : "</td>\n");
    if (command_group.count(CommandGroup::STANDARD_BEARER))
        row +=  std::string(command_group.count(CommandGroup::MUSICIAN) ? "" : "<td>") +
                command_group.at(CommandGroup::STANDARD_BEARER).first +
                (command_group.count(CommandGroup::CHAMPION) ? "<br/>" : "</td>\n");
    if (command_group.count(CommandGroup::CHAMPION))
        row +=  std::string((command_group.count(CommandGroup::MUSICIAN)
                             || command_group.count(CommandGroup::STANDARD_BEARER))
                            ? "" : "<td>") + command_group.at(CommandGroup::CHAMPION).first + "</td>\n";
    if (!command_group.count(CommandGroup::MUSICIAN) &&
            !command_group.count(CommandGroup::STANDARD_BEARER) &&
            !command_group.count(CommandGroup::CHAMPION)) row += "<td>&nbsp;</td>";
    // banner
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
    // => default member characteristics
    row += "<tr>\n";
    for (const auto& x : handle->statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    if (!handle->champion_statistics().empty() &&
            !std::equal(std::begin(handle->statistics()),
                    std::end(handle->statistics()),
                    std::begin(handle->champion_statistics()))) {
        // => champion member characteristics
        row += "<tr>\n";
        for (const auto& x : handle->champion_statistics())
            row += "<td align=\"center\">" + x + "</td>\n";
        row += "</tr>\n";
    }
    row += "</table></td>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}

std::string normal_unit::save() const {
    return "";
}
