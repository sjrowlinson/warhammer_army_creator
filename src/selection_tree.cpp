#include "selection_tree.h"

selection_tree::selection_tree(armies::Faction faction, army_list& list) : race(faction), army(list) {
    auto files = filenames();
    QString common_item_filename(":/magic_items/common.mag");
    tools::item_parser ip(common_item_filename, ItemClass::COMMON);
    auto items = ip.parse();
    common_items.first = ip.name();
    for (auto&& item : items) common_items.second[item.name] = item;
    QString rfile(std::get<0>(files).data());
    parse_roster_file(rfile);
    QString mifile(std::get<1>(files).data());
    QString fifile(std::get<2>(files).data());
    parse_item_files(std::make_pair(mifile, fifile));
}

std::tuple<std::string, std::string, std::string, std::string> selection_tree::filenames() const noexcept {
    std::string roster_file = ":/rosters/";
    std::string mounts_file = ":/rosters/";
    std::string magic_item_file = ":/magic_items/";
    std::string faction_item_file = ":/faction_items/";
    switch (race) {
    case armies::Faction::EMPIRE:
        roster_file += "empire.ros";
        magic_item_file += "empire.mag";
        break;
    case armies::Faction::BRETONNIA:
        roster_file += "bretonnia.ros";
        magic_item_file += "bretonnia.mag";
        break;
    case armies::Faction::DWARFS:
        roster_file += "dwarfs.ros";
        magic_item_file += "dwarfs.mag";
        break;
    case armies::Faction::HIGH_ELVES:
        roster_file += "high_elves.ros";
        magic_item_file += "high_elves.mag";
        break;
    case armies::Faction::WOOD_ELVES:
        roster_file += "wood_elves.ros";
        magic_item_file += "wood_elves.mag";
        break;
    case armies::Faction::DARK_ELVES:
        roster_file += "dark_elves.ros";
        magic_item_file += "dark_elves.mag";
        break;
    case armies::Faction::WARRIORS_OF_CHAOS:
        roster_file += "woc.ros";
        mounts_file += "woc.mnt";
        magic_item_file += "woc.mag";
        faction_item_file += "woc.fit";
        break;
    case armies::Faction::DAEMONS_OF_CHAOS:
        roster_file += "doc.ros";
        magic_item_file += "doc.mag";
        break;
    case armies::Faction::BEASTMEN:
        roster_file += "beastmen.ros";
        magic_item_file += "beastmen.mag";
        faction_item_file += "beastmen.fit";
        break;
    case armies::Faction::CHAOS_DWARFS:
        roster_file += "chaos_dwarfs.ros";
        magic_item_file += "chaos_dwarfs.mag";
        break;
    case armies::Faction::VAMPIRE_COUNTS:
        roster_file += "vampire_counts.ros";
        magic_item_file += "vampire_counts.mag";
        break;
    case armies::Faction::TOMB_KINGS:
        roster_file += "tomb_kings.ros";
        magic_item_file += "tomb_kings.mag";
        break;
    case armies::Faction::OGRE_KINGDOMS:
        roster_file += "ogre_kingdoms.ros";
        magic_item_file += "ogre_kingdoms.mag";
        break;
    case armies::Faction::SKAVEN:
        roster_file += "skaven.ros";
        mounts_file += "skaven.mnt";
        magic_item_file += "skaven.mag";
        faction_item_file += "skaven.fit";
        break;
    case armies::Faction::ORCS_AND_GOBLINS:
        roster_file += "orcs_goblins.ros";
        magic_item_file += "orcs_goblins.mag";
        break;
    default:
        roster_file += "empire.ros";
        magic_item_file += "empire.mag";
        break;
    }
    return {roster_file, mounts_file, magic_item_file, faction_item_file};
}

void selection_tree::change_selection(const std::string& name) {
    switch (roster[name]->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
        current_selection = std::make_shared<melee_character_unit>(melee_character_unit(roster[name]));
        break;
    case BaseUnitType::MAGE_CHARACTER:
        current_selection = std::make_shared<mage_character_unit>(mage_character_unit(roster[name]));
        break;
    case BaseUnitType::MIXED:
        current_selection = std::make_shared<mixed_unit>(mixed_unit(roster[name]));
        break;
    case BaseUnitType::NORMAL:
        current_selection = std::make_shared<normal_unit>(normal_unit(roster[name]));
        break;
    default: break;
    }
}

void selection_tree::reset_army_list(army_list& _army) {
    army = _army;
}

void selection_tree::reset(armies::Faction faction, army_list &list) {
    roster.clear();
    magic_items.first.clear();
    magic_items.second.clear();
    faction_items.first.clear();
    faction_items.second.clear();
    current_selection.reset();
    race = faction;
    auto files = filenames();
    QString rfile(std::get<0>(files).data());
    parse_roster_file(rfile);
    QString mfile(std::get<1>(files).data());
    parse_mount_file(mfile);
    QString mifile(std::get<2>(files).data());
    QString fifile(std::get<3>(files).data());
    parse_item_files(std::make_pair(mifile, fifile));
    reset_army_list(list);
}

std::shared_ptr<unit> selection_tree::selected() {
    switch (current_selection->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
        return std::dynamic_pointer_cast<melee_character_unit>(current_selection);
    case BaseUnitType::MAGE_CHARACTER:
        return std::dynamic_pointer_cast<mage_character_unit>(current_selection);
    case BaseUnitType::MIXED:
        return std::dynamic_pointer_cast<mixed_unit>(current_selection);
    case BaseUnitType::NORMAL:
        return std::dynamic_pointer_cast<normal_unit>(current_selection);
    default:
        return current_selection;
    }
}

void selection_tree::add_unit_to_army_list(int id) {
    current_selection->set_id(id);
    if (current_selection->name().empty()) return;
    army.get().add_unit(current_selection);
}

void selection_tree::parse_roster_file(const QString &rfile_str) {
    tools::roster_parser rp(rfile_str, race);
    auto units = rp.parse();
    for (auto&& x : units) roster[x->name()] = x;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > sp_common = std::make_shared<
                      std::pair<
                          std::string,
                          std::unordered_map<std::string, item>
                  >>(common_items);
    for (auto& entry : roster) entry.second->set_common_item_handle(sp_common);
}

void selection_tree::parse_mount_file(const QString& mfile_str) {
    tools::mounts_parser mp(mfile_str);
    auto mounts_vec = mp.parse();
    for (auto&& x : mounts_vec) mounts[x.name] = x;
    std::shared_ptr<
        std::unordered_map<
            std::string, mount
        >
    > sp_mounts = std::make_shared<
        std::unordered_map<
            std::string, mount
        >
    >(mounts);
    for (auto& entry : roster) entry.second->set_mounts_handle(sp_mounts);
}

void selection_tree::parse_item_files(const std::pair<QString, QString>& ifile_str) {
    // parse magic items file first
    tools::item_parser magic_items_parser(ifile_str.first, ItemClass::MAGIC);
    auto mag_items = magic_items_parser.parse();
    magic_items.first = magic_items_parser.name();
    for (auto&& item : mag_items) magic_items.second[item.name] = item;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > sp_items = std::make_shared<
                      std::pair<
                          std::string,
                          std::unordered_map<std::string, item>
                  >>(magic_items);
    for (auto& x : roster) x.second->set_magic_item_handle(sp_items);
    // then faction items file if it exists
    if (tools::split(ifile_str.second.toStdString(), '.').size() < 2U) return;
    tools::item_parser faction_items_parser(ifile_str.second, ItemClass::FACTION);
    auto fac_items = faction_items_parser.parse();
    faction_items.first = faction_items_parser.name();
    for (auto&& item : fac_items) faction_items.second[item.name] = item;
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > sp_fac_items = std::make_shared<
                      std::pair<
                          std::string,
                          std::unordered_map<std::string, item>
                  >>(faction_items);
    for (auto& entry : roster) entry.second->set_faction_item_handle(sp_fac_items);
}

std::string selection_tree::magic_items_name() const noexcept {
    return magic_items.first;
}

std::string selection_tree::faction_items_name() const noexcept {
    return faction_items.first;
}

std::vector<std::shared_ptr<base_unit>> selection_tree::all_of(armies::UnitType ut) const noexcept {
    std::vector<std::shared_ptr<base_unit>> v;
    for (const auto& x : roster) {
        if (x.second->unit_type() == ut) v.push_back(x.second);
    }
    return v;
}

std::vector<std::shared_ptr<base_unit>> selection_tree::lords() const noexcept {
    return all_of(armies::UnitType::LORD);
}
std::vector<std::shared_ptr<base_unit>> selection_tree::heroes() const noexcept {
    return all_of(armies::UnitType::HERO);
}
std::vector<std::shared_ptr<base_unit>> selection_tree::core() const noexcept {
    return all_of(armies::UnitType::CORE);
}
std::vector<std::shared_ptr<base_unit>> selection_tree::special() const noexcept {
    return all_of(armies::UnitType::SPECIAL);
}
std::vector<std::shared_ptr<base_unit>> selection_tree::rare() const noexcept {
    return all_of(armies::UnitType::RARE);
}
