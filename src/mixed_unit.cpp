#include "mixed_unit.h"

mixed_unit::mixed_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle)
    : unit(base, army_handle),
      master_size_(0U), slave_size_(0U),
      handle(std::dynamic_pointer_cast<base_mixed_unit>(base)) {

}


/*void mixed_unit::change_master_size(std::size_t n) {
    if (static_cast<double>(n)/slave_.size() > handle->ratio_limit())
        throw std::invalid_argument("Master:Slave size ratio limit exceeded!");
    master_.change_size(n);
}

void mixed_unit::change_slave_size(std::size_t n) {
    if ( master_.size()/static_cast<double>(n) > handle->ratio_limit())
        throw std::invalid_argument("Master:Slave size ratio limit exceeded!");
    slave_.change_size(n);
}*/

/*std::string mixed_unit::html_table_row() const {
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
}*/
