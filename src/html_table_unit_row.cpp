#include "html_table_unit_row.h"

html_table_unit_row::html_table_unit_row(const std::shared_ptr<unit>& _u)
    : u(_u), urow() {
    if (u->is_character()) make_character_row();
    else {
        if (u->is_mixed()) make_mixed_unit_row();
        else make_normal_unit_row();
    }
}

void html_table_unit_row::make_character_row() {
    // name
    urow += row_entry(u->full_name());
    // mount
    if (std::get<0>(u->mnt()).empty())
        urow += row_entry("");
    else {
        std::string entry = std::get<0>(u->mnt());
        if (!std::get<2>(u->mnt()).first.empty())
            entry += newline + tab + std::get<2>(u->mnt()).first;
        for (const auto& x : std::get<3>(u->mnt()))
            entry += newline + tab + x.first;
        urow += row_entry(entry);
    }
    // mage level
    if (u->is_mage()) {
        urow += row_entry(
            "Level " + std::to_string(std::dynamic_pointer_cast<mage_character_unit>(u)->level())
        );
    } else urow += row_entry("");
    // weapons
    auto w = u->weapons();
    if (w.empty()) urow += row_entry("");
    else {
        std::string entry = "";
        if (w.count(WeaponType::MELEE))
            entry += bold("Melee: ") + std::get<1>(w.at(WeaponType::MELEE));
        if (w.count(WeaponType::BALLISTIC))
            entry += (w.count(WeaponType::MELEE) ? newline : "") + bold("Ranged: ")
                    + std::get<1>(w.at(WeaponType::BALLISTIC));
        urow += row_entry(entry);
    }
    // armour
    auto a = u->armour();
    if (a.empty()) urow += row_entry("");
    else {
        std::string entry = "";
        if (a.count(ArmourType::ARMOUR))
            entry += bold("Body: ") + std::get<1>(a.at(ArmourType::ARMOUR));
        if (a.count(ArmourType::SHIELD))
            entry += (a.count(ArmourType::ARMOUR) ? newline : "") + bold("Shield: ")
                    + std::get<1>(a.at(ArmourType::SHIELD));
        if (a.count(ArmourType::HELMET))
            entry += (a.count(ArmourType::ARMOUR) || a.count(ArmourType::SHIELD) ? newline : "")
                    + bold("Helmet: ") + std::get<1>(a.at(ArmourType::HELMET));
        urow += row_entry(entry);
    }
    // talisman
    if (u->talisman().first.empty()) urow += row_entry("");
    else urow += row_entry(u->talisman().first);
    // enchanted item
    if (u->enchanted_item().first.empty()) urow += row_entry("");
    else urow += row_entry(u->enchanted_item().first);
    // arcane item
    if (u->arcane_item().first.empty()) urow += row_entry("");
    else urow += row_entry(u->arcane_item().first);
    // magic extras
    if (u->item_extras().empty()) urow += row_entry("");
    else {
        std::string entry = "";
        for (const auto& x : u->item_extras()) entry += x.first + newline;
        urow += row_entry(entry);
    }
    // other extras
    if (u->oco_extra().first.empty() && u->mc_extras().empty()) urow += row_entry("");
    else {
        std::string entry = u->oco_extra().first;
        for (const auto& x : u->mc_extras()) entry += (entry.empty() ? "" : newline) + x.first;
        urow += row_entry(entry);
    }
    // banner
    if (u->magic_banner().first.empty()) urow += row_entry("");
    else urow += row_entry(u->magic_banner().first);
    // characteristics
    auto uc = std::dynamic_pointer_cast<character_unit>(u);
    urow += characteristics_table(
        uc->handle_->statistics(),
        {},
        !std::get<0>(uc->mnt()).empty() ?
            uc->handle_->mounts_handle()->find(std::get<0>(uc->mnt()))->second.statistics() : std::vector<std::string>{}
    );
    // points
    urow += row_entry(tools::points_str(u->points()));
    // choose correct colour based on lord or hero unit type
    std::string colour;
    switch (u->unit_type()) {
    case UnitType::LORD:
        colour = "#D7BDE2";
        break;
    case UnitType::HERO:
        colour = "#FDEBD0";
        break;
    default: break;
    }
    urow = row(urow, " style=\"background-color:" + colour + "\"");
}

void html_table_unit_row::make_normal_unit_row() {
    auto p = std::dynamic_pointer_cast<normal_unit>(u);
    if (p == nullptr)
        throw std::runtime_error("Bug encountered! Unable to dynamically cast unit ptr to normal_unit ptr"
                                 " in html_table_unit_row::make_normal_unit_row");
    // store champion name for convenience
    std::string champ_name;
    auto champ_it = p->handle->optional_command().find(CommandGroup::CHAMPION);
    if (champ_it != std::end(p->handle->optional_command())) champ_name = champ_it->second.first;
    // name
    urow += row_entry(p->full_name());
    // size
    urow += row_entry(std::to_string(p->size()));
    // mount
    if (std::get<0>(p->mnt()).empty())
        urow += row_entry("");
    else {
        std::string entry = std::get<0>(p->mnt());
        if (!std::get<2>(p->mnt()).first.empty())
            entry += newline + tab + std::get<2>(p->mnt()).first;
        for (const auto& x : std::get<3>(p->mnt()))
            entry += newline + tab + x.first;
        urow += row_entry(entry);
    }
    // weapons
    auto w = p->weapons();
    auto cw = p->champion_weapons();
    if (w.empty() && cw.empty()) urow += row_entry("");
    else if (cw.empty()) {
        std::string entry = "";
        if (w.count(WeaponType::MELEE))
            entry += bold("Melee: ") + std::get<1>(w.at(WeaponType::MELEE));
        if (w.count(WeaponType::BALLISTIC))
            entry += (w.count(WeaponType::MELEE) ? newline : "") + bold("Ranged: ")
                    + std::get<1>(w.at(WeaponType::BALLISTIC));
        urow += row_entry(entry);
    }
    else {
        std::string entry = (w.empty() ? champ_name : p->name()) + newline;
        if (!w.empty()) {
            if (w.count(WeaponType::MELEE))
                entry += tab + bold("Melee: ") + std::get<1>(w.at(WeaponType::MELEE));
            if (w.count(WeaponType::BALLISTIC))
                entry += (w.count(WeaponType::MELEE) ? newline : "") + tab + bold("Ranged: ")
                        + std::get<1>(w.at(WeaponType::BALLISTIC));
            entry += newline + champ_name + newline;
        }
        if (cw.count(WeaponType::MELEE))
            entry += tab + bold("Melee: ") + std::get<1>(cw.at(WeaponType::MELEE));
        if (cw.count(WeaponType::BALLISTIC))
                entry += (cw.count(WeaponType::MELEE) ? newline : "") + tab + bold("Ranged: ")
                        + std::get<1>(cw.at(WeaponType::BALLISTIC));
        urow += row_entry(entry);
    }
    // armour
    auto a = p->armour();
    auto ca = p->champion_armour();
    if (a.empty() && ca.empty()) urow += row_entry("");
    else if (ca.empty()) {
        std::string entry = "";
        if (a.count(ArmourType::ARMOUR))
            entry += bold("Body: ") + std::get<1>(a.at(ArmourType::ARMOUR));
        if (a.count(ArmourType::SHIELD))
            entry += (a.count(ArmourType::ARMOUR) ? newline : "") + bold("Shield: ")
                    + std::get<1>(a.at(ArmourType::SHIELD));
        if (a.count(ArmourType::HELMET))
            entry += (a.count(ArmourType::ARMOUR) || a.count(ArmourType::SHIELD) ? newline : "")
                    + bold("Helmet: ") + std::get<1>(a.at(ArmourType::HELMET));
        urow += row_entry(entry);
    }
    else {
        std::string entry = (a.empty() ? champ_name : p->name()) + newline;
        if (!a.empty()) {
            if (a.count(ArmourType::ARMOUR))
                entry += tab + bold("Body: ") + std::get<1>(a.at(ArmourType::ARMOUR));
            if (a.count(ArmourType::SHIELD))
                entry += (a.count(ArmourType::ARMOUR) ? newline : "") + tab + bold("Shield: ")
                        + std::get<1>(a.at(ArmourType::SHIELD));
            if (a.count(ArmourType::HELMET))
                entry += (a.count(ArmourType::ARMOUR) || a.count(ArmourType::SHIELD) ? newline : "")
                        + tab + bold("Helmet: ") + std::get<1>(a.at(ArmourType::HELMET));
            entry += newline + champ_name + newline;
        }
        if (ca.count(ArmourType::ARMOUR))
                entry += tab + bold("Body: ") + std::get<1>(ca.at(ArmourType::ARMOUR));
        if (ca.count(ArmourType::SHIELD))
            entry += (ca.count(ArmourType::ARMOUR) ? newline : "") + tab + bold("Shield: ")
                    + std::get<1>(ca.at(ArmourType::SHIELD));
        if (ca.count(ArmourType::HELMET))
            entry += (ca.count(ArmourType::ARMOUR) || ca.count(ArmourType::SHIELD) ? newline : "")
                    + tab + bold("Helmet: ") + std::get<1>(ca.at(ArmourType::HELMET));
        urow += row_entry(entry);
    }
    // extras
    auto oco = p->oco_extra();
    auto coco = p->champion_oco_extra();
    auto mc = p->mc_extras();
    auto cmc = p->champion_mc_extras();
    auto ctalisman = p->talisman();
    auto cenchanted = p->enchanted_item();
    auto carcane = p->arcane_item();
    auto cextras = p->item_extras();
    if (oco.first.empty() && coco.first.empty() && mc.empty() && cmc.empty()
            && ctalisman.first.empty() && cenchanted.first.empty()
            && carcane.first.empty() && cextras.empty()) urow += row_entry("");
    else if (coco.first.empty() && cmc.empty()
                && ctalisman.first.empty() && cenchanted.first.empty()
                && carcane.first.empty() && cextras.empty()) { // no champion extras only defaults
        std::string entry = oco.first;
        for (const auto& x : mc) entry += (entry.empty() ? "" : newline) + x.first;
        urow += row_entry(entry);
    }
    else {
        std::string entry = (oco.first.empty() && mc.empty() ? champ_name : p->name()) + newline;
        if (!oco.first.empty()) entry += tab + oco.first;
        for (const auto& x : mc) entry += (oco.first.empty() ? newline : "") + tab + x.first;
        if (!oco.first.empty() || !mc.empty()) entry += newline + champ_name + newline;
        if (!coco.first.empty()) entry += tab + coco.first;
        for (const auto& x : cmc) entry += (coco.first.empty() ? newline : "") + tab + x.first;
        if (!ctalisman.first.empty()) entry += newline + tab + ctalisman.first;
        if (!cenchanted.first.empty()) entry += newline + tab + cenchanted.first;
        if (!carcane.first.empty()) entry += newline + tab + carcane.first;
        for (const auto& x : cextras) entry += newline + tab + x.first;
        urow += row_entry(entry);
    }
    // command
    if (!p->command().count(CommandGroup::MUSICIAN) && !p->command().count(CommandGroup::STANDARD_BEARER) &&
            !p->command().count(CommandGroup::CHAMPION))
        urow += row_entry("");
    else {
        std::string entry = "";
        if (p->command().count(CommandGroup::STANDARD_BEARER))
            entry += p->command().at(CommandGroup::STANDARD_BEARER).first;
        if (p->command().count(CommandGroup::CHAMPION))
            entry += (entry.empty() ? "" : newline) + p->command().at(CommandGroup::CHAMPION).first;
        if (p->command().count(CommandGroup::MUSICIAN))
            entry += (entry.empty() ? "" : newline) + p->command().at(CommandGroup::MUSICIAN).first;
        urow += row_entry(entry);
    }
    // banner
    if (p->magic_banner().first.empty()) urow += row_entry("");
    else urow += row_entry(u->magic_banner().first);
    // characteristics
    urow += characteristics_table(
        p->handle->statistics(),
        p->command().count(CommandGroup::CHAMPION) ? p->handle->champion_statistics() : std::vector<std::string>{},
        !std::get<0>(p->mnt()).empty() ?
            p->handle->mounts_handle()->find(std::get<0>(p->mnt()))->second.statistics() : std::vector<std::string>{}
    );
    // points
    urow += row_entry(tools::points_str(p->points()));
    // choose correct colour based on core, special or rare unit type
    std::string colour;
    switch (p->unit_type()) {
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
    urow = row(urow, " style=\"background-color:" + colour + "\"");
}

void html_table_unit_row::make_mixed_unit_row() {
    auto p = std::dynamic_pointer_cast<mixed_unit>(u);
    if (p == nullptr)
        throw std::runtime_error("Bug encountered! Unable to dynamically cast unit ptr to mixed_unit ptr"
                                 " in html_table_unit_row::make_mixed_unit_row");
    // store some names for convenience
    std::string mname = p->handle->master_name();
    std::string sname = p->handle->slave_name();
    std::string mcname;
    auto mchamp_it = p->handle->master_optional_command().find(CommandGroup::CHAMPION);
    if (mchamp_it != std::end(p->handle->master_optional_command())) mcname = mchamp_it->second.first;
    std::string scname;
    auto schamp_it = p->handle->slave_optional_command().find(CommandGroup::CHAMPION);
    if (schamp_it != std::end(p->handle->slave_optional_command())) scname = schamp_it->second.first;
    // NAME
    urow += row_entry(p->full_name());
    // SIZE
    urow += row_entry(std::to_string(p->slave_size()) + ' ' + sname + newline
                      + std::to_string(p->master_size()) + ' ' + mname + (p->master_size() > 1 ? "s" : ""));
    // MOUNT
    if (std::get<0>(p->mnt()).empty()) urow += row_entry("");
    else {
        std::string entry = std::get<0>(p->mnt());
        if (!std::get<2>(p->mnt()).first.empty())
            entry += newline + tab + std::get<2>(p->mnt()).first;
        for (const auto& x : std::get<3>(p->mnt()))
            entry += newline + tab + x.first;
        urow += row_entry(entry);
    }
    // WEAPONS
    /*auto sw = p->slave_weapons();
    auto scw = p->slave_champion_weapons();
    auto mw = p->master_weapons();
    auto mcw = p->master_champion_weapons();*/
    // TODO: this could get very messy, think about clean implementation for different permutations/combinations
    urow += row_entry("");
    // ARMOUR
    /*auto sa = p->slave_armour();
    auto sca = p->slave_champion_armour();
    auto ma = p->master_armour();
    auto mca =p->master_champion_armour();*/
    // TODO: this could get very messy, think about clean implementation for different permutations/combinations
    urow += row_entry("");
    // EXTRAS
    // TODO: this could get very messy, think about clean implementation for different permutations/combinations
    urow += row_entry("");
    // COMMAND
    auto scomm = p->slave_command();
    auto mcomm = p->master_command();
    if (!scomm.count(CommandGroup::MUSICIAN) && !scomm.count(CommandGroup::STANDARD_BEARER)
            && !scomm.count(CommandGroup::CHAMPION) && !mcomm.count(CommandGroup::MUSICIAN)
            && !mcomm.count(CommandGroup::STANDARD_BEARER) && !mcomm.count(CommandGroup::CHAMPION))
        urow += row_entry("");
    else {
        std::string entry = (scomm.empty() ? mname : sname) + newline;
        if (scomm.count(CommandGroup::STANDARD_BEARER))
            entry += tab + scomm.at(CommandGroup::STANDARD_BEARER).first;
        if (scomm.count(CommandGroup::CHAMPION))
            entry += (scomm.count(CommandGroup::STANDARD_BEARER) ? newline : "") + tab
                    + scomm.at(CommandGroup::CHAMPION).first;
        if (scomm.count(CommandGroup::MUSICIAN))
            entry += (scomm.count(CommandGroup::STANDARD_BEARER) || scomm.count(CommandGroup::CHAMPION) ? newline : "")
                    + tab + scomm.at(CommandGroup::CHAMPION).first;
        if (!scomm.empty() && !mcomm.empty()) entry += newline + mname + newline;
        if (mcomm.count(CommandGroup::STANDARD_BEARER))
            entry += tab + mcomm.at(CommandGroup::STANDARD_BEARER).first;
        if (mcomm.count(CommandGroup::CHAMPION))
            entry += (mcomm.count(CommandGroup::STANDARD_BEARER) ? newline : "") + tab
                    + mcomm.at(CommandGroup::CHAMPION).first;
        if (mcomm.count(CommandGroup::MUSICIAN))
            entry += (mcomm.count(CommandGroup::STANDARD_BEARER) || mcomm.count(CommandGroup::CHAMPION) ? newline : "")
                    + tab + mcomm.at(CommandGroup::CHAMPION).first;
        urow += row_entry(entry);
    }
    // BANNER
    if (p->magic_banner().first.empty()) urow += row_entry("");
    else urow += row_entry(u->magic_banner().first);
    // CHARACTERISTICS
    urow += mixed_characteristics_table(
        p->handle->slave_statistics(),
        p->slave_command().count(CommandGroup::CHAMPION) ?
                    p->handle->slave_champion_statistics() : std::vector<std::string>{},
        p->handle->master_statistics(),
        p->master_command().count(CommandGroup::CHAMPION) ?
                    p->handle->master_champion_statistics() : std::vector<std::string>{}
    );
    // POINTS
    urow += row_entry(tools::points_str(p->points()));
    // choose correct colour based on core, special or rare unit type
    std::string colour;
    switch (p->unit_type()) {
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
    urow = row(urow, " style=\"background-color:" + colour + "\"");
}

const std::string& html_table_unit_row::to_string() const noexcept {
    return urow;
}

std::string html_table_unit_row::row(const std::string& s, std::string args) {
    return "<tr" + args + '>' + (s.empty() ? "&nbsp;" : s) + "</tr>\n";
}

std::string html_table_unit_row::row_entry(const std::string& s, std::string args) {
    return "<td" + args + '>' + (s.empty() ? "&nbsp;" : s) + "</td>\n";
}

std::string html_table_unit_row::bold(const std::string& s) {
    return "<strong>" + s + "</strong>";
}

std::string html_table_unit_row::characteristics_table(
    const std::vector<std::string>& chars,
    const std::vector<std::string>& champ_chars,
    const std::vector<std::string>& mount_chars
) {
    std::string table = "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    table += "<thead><tr>\n"
                "<th>&nbsp;</th>"
                "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
                "<th>I</th><th>A</th><th>Ld</th>\n"
             "</tr></thead>\n";
    table += "<tr>\n";
    table += row_entry(u->name(), " max-width:100% white-space:nowrap");
    for (const auto& x : chars) table += row_entry(x, " align=\"center\"");
    table += "</tr>\n";
    if (!champ_chars.empty() && !std::equal(
        std::begin(chars), std::end(chars), std::begin(champ_chars)
    )) {
        table += "<tr>\n";
        table += row_entry(
                    std::dynamic_pointer_cast<normal_unit>(u)->handle->optional_command().at(CommandGroup::CHAMPION).first,
                    " max-width:100% white-space:nowrap"
                 );
        for (const auto& x : champ_chars) table += row_entry(x, " align=\"center\"");
        table += "</tr>\n";
    }
    if (!mount_chars.empty()) {
        table += "<tr>\n";
        table += row_entry(std::get<0>(u->mnt()), " max-width:100% white-space:nowrap");
        for (const auto& x : mount_chars) table += row_entry(x, " align=\"center\"");
        table += "</tr>\n";
    }
    table += "</table></td>\n";
    return table;
}

std::string html_table_unit_row::mixed_characteristics_table(
    const std::vector<std::string>& slave_chars,
    const std::vector<std::string>& slave_champ_chars,
    const std::vector<std::string>& master_chars,
    const std::vector<std::string>& master_champ_chars
        ) {
    auto p = std::dynamic_pointer_cast<mixed_unit>(u);
    std::string table = "<td><table border=1 cellspacing=0 cellpadding=1 width=100%>\n";
    table += "<thead><tr>\n"
                "<th>&nbsp;</th>"
                "<th>M</th><th>WS</th><th>BS</th><th>S</th><th>T</th><th>W</th>"
                "<th>I</th><th>A</th><th>Ld</th>\n"
             "</tr></thead>\n";
    table += "<tr>\n";
    table += row_entry(p->handle->slave_name(), " max-width:100% white-space:nowrap");
    for (const auto& x : slave_chars) table += row_entry(x, " align=\"center\"");
    table += "</tr>\n";
    if (!slave_champ_chars.empty() && !std::equal(
        std::begin(slave_chars), std::end(slave_chars), std::begin(slave_champ_chars)
    )) {
        table += "<tr>\n";
        table += row_entry(
            p->handle->slave_optional_command().at(CommandGroup::CHAMPION).first,
            " max-width:100% white-space:nowrap"
        );
        for (const auto& x : slave_champ_chars) table += row_entry(x, " align=\"center\"");
        table += "</tr>\n";
    }
    table += "<tr>\n";
    table += row_entry(p->handle->master_name(), " max-width:100% white-space:nowrap");
    for (const auto& x : master_chars) table += row_entry(x, " align=\"center\"");
    table += "</tr>\n";
    if (!master_champ_chars.empty() && !std::equal(
        std::begin(master_chars), std::end(master_chars), std::begin(master_champ_chars)
    )) {
        table += "<tr>\n";
        table += row_entry(
            p->handle->master_optional_command().at(CommandGroup::CHAMPION).first,
            " max-width:100% white-space:nowrap"
        );
        for (const auto& x : master_champ_chars) table += row_entry(x, " align=\"center\"");
        table += "</tr>\n";
    }
    table += "</table></td>\n";
    return table;
}


