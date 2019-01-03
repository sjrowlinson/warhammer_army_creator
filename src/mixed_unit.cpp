#include "mixed_unit.h"

mixed_unit::mixed_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : unit(base, army_handle),
      handle(std::dynamic_pointer_cast<base_mixed_unit>(base)),
      master_(std::make_shared<base_normal_unit>(handle->master()), army_handle),
      slave_(std::make_shared<base_normal_unit>(handle->slave()), army_handle) {
    mixed_select_ = MixedSelect::SLAVE;
}

mixed_unit::mixed_unit(const mixed_unit& other)
    : unit(other), handle(other.handle), master_(other.master_),
      slave_(other.slave_),
      master_size_(other.master_size_), slaves_size_(other.slaves_size_) {}

bool mixed_unit::is_character() const noexcept { return false; }
bool mixed_unit::is_mage() const noexcept { return false; }
bool mixed_unit::is_mixed() const noexcept { return true; }

bool mixed_unit::switch_model_select(ModelSelect ms) {
    model_select_ = ms;
    return true;
}
bool mixed_unit::switch_mixed_select(MixedSelect ms) {
    mixed_select_ = ms;
    return true;
}

normal_unit& mixed_unit::master() noexcept { return master_; }
normal_unit& mixed_unit::slave() noexcept { return slave_; }

double mixed_unit::points() const noexcept {
    return master_.points() + slave_.points();
}

const std::unordered_map<
    WeaponType,
    std::tuple<ItemCategory, std::string, double>
>& mixed_unit::weapons() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.weapons();
    case MixedSelect::SLAVE:
        return slave_.weapons();
    }
    return master_.weapons();
}

const std::unordered_map<
    ArmourType,
    std::tuple<ItemCategory, std::string, double>
>& mixed_unit::armour() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.armour();
    case MixedSelect::SLAVE:
        return slave_.armour();
    }
    return master_.armour();
}

const std::pair<
    std::string,
    std::pair<bool, double>
>& mixed_unit::oco_extra() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.oco_extra();
    case MixedSelect::SLAVE:
        return slave_.oco_extra();
    }
    return master_.oco_extra();
}

const std::unordered_map<
    std::string,
    std::pair<bool, double>
>& mixed_unit::mc_extras() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.mc_extras();
    case MixedSelect::SLAVE:
        return slave_.mc_extras();
    }
    return master_.mc_extras();
}

const std::tuple<
    std::string,
    double,
    std::pair<std::string, double>,
    std::unordered_map<std::string, double>
>& mixed_unit::mnt() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.mnt();
    case MixedSelect::SLAVE:
        return slave_.mnt();
    }
    return master_.mnt();
}

const std::pair<std::string, std::pair<ItemCategory, double>>& mixed_unit::magic_banner() const noexcept {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.magic_banner();
    case MixedSelect::SLAVE:
        return slave_.magic_banner();
    }
    return master_.magic_banner();
}

std::string mixed_unit::pick_weapon(ItemCategory item_type, const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.pick_weapon(item_type, name);
    case MixedSelect::SLAVE:
        return slave_.pick_weapon(item_type, name);
    }
    return master_.pick_weapon(item_type, name);
}

std::string mixed_unit::remove_weapon(WeaponType wt, bool replacing) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.remove_weapon(wt, replacing);
    case MixedSelect::SLAVE:
        return slave_.remove_weapon(wt, replacing);
    }
    return master_.remove_weapon(wt, replacing);
}

std::string mixed_unit::pick_armour(ItemCategory item_type, const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.pick_armour(item_type, name);
    case MixedSelect::SLAVE:
        return slave_.pick_armour(item_type, name);
    }
    return master_.pick_armour(item_type, name);
}

std::string mixed_unit::remove_armour(ArmourType at, bool replacing) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.remove_armour(at, replacing);
    case MixedSelect::SLAVE:
        return slave_.remove_armour(at, replacing);
    }
    return master_.remove_armour(at, replacing);
}

std::string mixed_unit::pick_oco_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.pick_oco_extra(name);
    case MixedSelect::SLAVE:
        return slave_.pick_oco_extra(name);
    }
    return master_.pick_oco_extra(name);
}

std::string mixed_unit::remove_oco_extra() {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.remove_oco_extra();
    case MixedSelect::SLAVE:
        return slave_.remove_oco_extra();
    }
    return master_.remove_oco_extra();
}

std::string mixed_unit::pick_mc_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.pick_mc_extra(name);
    case MixedSelect::SLAVE:
        return slave_.pick_mc_extra(name);
    }
    return master_.pick_mc_extra(name);
}

std::string mixed_unit::remove_mc_extra(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.remove_mc_extra(name);
    case MixedSelect::SLAVE:
        return slave_.remove_mc_extra(name);
    }
    return master_.remove_mc_extra(name);
}

void mixed_unit::pick_mount(const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.pick_mount(name);
        break;
    case MixedSelect::SLAVE:
        slave_.pick_mount(name);
        break;
    }
}

void mixed_unit::remove_mount() {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        master_.remove_mount();
        break;
    case MixedSelect::SLAVE:
        slave_.remove_mount();
        break;
    }
}

std::string mixed_unit::pick_banner(ItemCategory item_class, const std::string& name) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.pick_banner(item_class, name);
    case MixedSelect::SLAVE:
        return slave_.pick_banner(item_class, name);;
    }
    return master_.pick_banner(item_class, name);
}

std::string mixed_unit::remove_banner() {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        return master_.remove_banner();
    case MixedSelect::SLAVE:
        return slave_.remove_banner();
    }
    return master_.remove_banner();
}

void mixed_unit::change_size(std::size_t n) {
    switch (mixed_select_) {
    case MixedSelect::MASTER:
        try { change_master_size(n); }
        catch (const std::invalid_argument&) { throw ; }
        break;
    case MixedSelect::SLAVE:
        try { change_slave_size(n); }
        catch (const std::invalid_argument&) { throw ; }
        break;
    }
}

void mixed_unit::change_master_size(std::size_t n) {
    if (static_cast<double>(n)/slave_.size() > handle->ratio_limit())
        throw std::invalid_argument("Master:Slave size ratio limit exceeded!");
    master_.change_size(n);
}

void mixed_unit::change_slave_size(std::size_t n) {
    if ( master_.size()/static_cast<double>(n) > handle->ratio_limit())
        throw std::invalid_argument("Master:Slave size ratio limit exceeded!");
    slave_.change_size(n);
}

std::string mixed_unit::html_table_row() const {
    std::string row = "<tr>";
    // unit name
    row += "<td>" + name() + "</td>\n";
    // unit size
    row += "<td>" + std::to_string(master_.size()) + " Packmasters<br/>" +
            std::to_string(slave_.size()) + " " + name() + "</td>\n";
    // unit mount
    row += "<td>&nbsp;</td>\n";
    // weapons
    if (slave_.weapons().count(WeaponType::MELEE))
        row += "<td><strong>Melee:</strong> " +
                std::get<1>(slave_.weapons().at(WeaponType::MELEE)) +
                (slave_.weapons().count(WeaponType::BALLISTIC) ? "<br/>" : "</td>\n");
    if (slave_.weapons().count(WeaponType::BALLISTIC))
        row +=  std::string(slave_.weapons().count(WeaponType::MELEE) ? "" : "<td>") + "Ranged: " +
                std::get<1>(slave_.weapons().at(WeaponType::BALLISTIC)) + "</td>\n";
    if (!slave_.weapons().count(WeaponType::MELEE) &&
            !slave_.weapons().count(WeaponType::BALLISTIC)) row += "<td>&nbsp;</td>";
    // armour
    if (slave_.armour().count(ArmourType::ARMOUR))
        row += "<td>Body: " +
                std::get<1>(slave_.armour().at(ArmourType::ARMOUR)) +
                (slave_.armour().count(ArmourType::SHIELD) || slave_.armour().count(ArmourType::HELMET)
                    ? "<br/>" : "</td>\n");
    if (slave_.armour().count(ArmourType::SHIELD))
        row +=  std::string(slave_.armour().count(ArmourType::ARMOUR) ? "" : "<td>") + "Shield: " +
                std::get<1>(slave_.armour().at(ArmourType::SHIELD)) +
                (slave_.armour().count(ArmourType::HELMET) ? "<br/>" : "</td>\n");
    if (slave_.armour().count(ArmourType::HELMET))
        row +=  std::string((slave_.armour().count(ArmourType::ARMOUR)
                             || slave_.armour().count(ArmourType::SHIELD))
                            ? "" : "<td>") + "Helmet: " +
                std::get<1>(slave_.armour().at(ArmourType::HELMET)) + "</td>\n";
    if (!slave_.armour().count(ArmourType::ARMOUR) &&
            !slave_.armour().count(ArmourType::SHIELD) &&
            !slave_.armour().count(ArmourType::HELMET)) row += "<td>&nbsp;</td>";
    // extras
    row += "<td>";
    if (!slave_.oco_extra().first.empty()) row += slave_.oco_extra().first + (slave_.mc_extras().empty() ? "" : "<br/>");
    for (const auto& x : slave_.mc_extras()) row += x.first + "<br/>";
    if (slave_.oco_extra().first.empty() && slave_.mc_extras().empty()) row += "&nbsp;";
    row += "</td>\n";
    // command
    if (slave_.command().count(CommandGroup::MUSICIAN))
        row += "<td>" +
                slave_.command().at(CommandGroup::MUSICIAN).first +
                (slave_.command().count(CommandGroup::STANDARD_BEARER)
                 || slave_.command().count(CommandGroup::CHAMPION)
                    ? "<br/>" : "</td>\n");
    if (slave_.command().count(CommandGroup::STANDARD_BEARER))
        row +=  std::string(slave_.command().count(CommandGroup::MUSICIAN) ? "" : "<td>") +
                slave_.command().at(CommandGroup::STANDARD_BEARER).first +
                (slave_.command().count(CommandGroup::CHAMPION) ? "<br/>" : "</td>\n");
    if (slave_.command().count(CommandGroup::CHAMPION))
        row +=  std::string((slave_.command().count(CommandGroup::MUSICIAN)
                             || slave_.command().count(CommandGroup::STANDARD_BEARER))
                            ? "" : "<td>") + slave_.command().at(CommandGroup::CHAMPION).first + "</td>\n";
    if (!slave_.command().count(CommandGroup::MUSICIAN) &&
            !slave_.command().count(CommandGroup::STANDARD_BEARER) &&
            !slave_.command().count(CommandGroup::CHAMPION)) row += "<td>&nbsp;</td>";
    // banner
    row += "<td>";
    if (!slave_.magic_banner().first.empty()) row += slave_.magic_banner().first;
    else row += "&nbsp;";
    row += "</td>\n";
    // characteristics
    row += "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    row += "<thead><tr>\n"
            "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
            "<th>I</th><th>A</th><th>Ld</th>\n"
           "</tr></thead>\n";
    // => slave characteristics
    row += "<tr>\n";
    for (const auto& x : handle->slave().statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    // => master characteristics
    row += "<tr>\n";
    for (const auto& x : handle->master().statistics()) row += "<td align=\"center\">" + x + "</td>\n";
    row += "</tr>\n";
    // points
    row += "<td>" + tools::points_str(points()) + "</td>\n";
    // end row
    row += "</tr>\n";
    return row;
}

std::string mixed_unit::save() const {
    return "";
}
