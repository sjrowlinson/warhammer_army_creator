#include "selection_tree.h"

selection_tree::selection_tree(armies::Faction faction, army_list& list) : race(faction), army(list) {
    auto files = filenames();
    QString rfile(files.first.data());
    parse_roster_file(rfile);
    QString mifile(files.second.data());
    parse_item_file(mifile);
}

std::pair<std::string, std::string> selection_tree::filenames() const noexcept {
    std::string roster_file = ":/rosters/";
    std::string item_file = ":/magic_items/";
    switch (race) {
    case armies::Faction::EMPIRE:
        roster_file += "empire.ros";
        item_file += "empire.mag";
        break;
    case armies::Faction::BRETONNIA:
        roster_file += "bretonnia.ros";
        item_file += "bretonnia.mag";
        break;
    case armies::Faction::DWARFS:
        roster_file += "dwarfs.ros";
        item_file += "dwarfs.mag";
        break;
    case armies::Faction::HIGH_ELVES:
        roster_file += "high_elves.ros";
        item_file += "high_elves.mag";
        break;
    case armies::Faction::WOOD_ELVES:
        roster_file += "wood_elves.ros";
        item_file += "wood_elves.mag";
        break;
    case armies::Faction::DARK_ELVES:
        roster_file += "dark_elves.ros";
        item_file += "dark_elves.mag";
        break;
    case armies::Faction::WARRIORS_OF_CHAOS:
        roster_file += "woc.ros";
        item_file += "woc.mag";
        break;
    case armies::Faction::DAEMONS_OF_CHAOS:
        roster_file += "doc.ros";
        item_file += "doc.mag";
        break;
    case armies::Faction::BEASTMEN:
        roster_file += "beastmen.ros";
        item_file += "beastmen.mag";
        break;
    case armies::Faction::CHAOS_DWARFS:
        roster_file += "chaos_dwarfs.ros";
        item_file += "chaos_dwarfs.mag";
        break;
    case armies::Faction::VAMPIRE_COUNTS:
        roster_file += "vampire_counts.ros";
        item_file += "vampire_counts.mag";
        break;
    case armies::Faction::TOMB_KINGS:
        roster_file += "tomb_kings.ros";
        item_file += "tomb_kings.mag";
        break;
    case armies::Faction::OGRE_KINGDOMS:
        roster_file += "ogre_kingdoms.ros";
        item_file += "ogre_kingdoms.mag";
        break;
    case armies::Faction::SKAVEN:
        roster_file += "skaven.ros";
        item_file += "skaven.mag";
        break;
    case armies::Faction::ORCS_AND_GOBLINS:
        roster_file += "orcs_goblins.ros";
        item_file += "orcs_goblins.mag";
        break;
    default:
        roster_file += "empire.ros";
        item_file += "empire.mag";
        break;
    }
    return {roster_file, item_file};
}

void selection_tree::change_selection(const std::string& name) {
    current_selection = unit(roster[name]);
}

void selection_tree::reset_army_list(army_list& _army) {
    army = _army;
}

void selection_tree::reset(armies::Faction faction, army_list &list) {
    roster.clear();
    magic_items.clear();
    current_selection = unit();
    race = faction;
    auto files = filenames();
    QString rfile(files.first.data());
    parse_roster_file(rfile);
    QString mifile(files.second.data());
    parse_item_file(mifile);
    reset_army_list(list);
}

unit& selection_tree::selected() {
    return current_selection;
}


void selection_tree::add_unit_to_army_list(int id) {
    current_selection.set_id(id);
    if (current_selection.name().empty()) return;
    army.get().add_unit(current_selection);
}

void selection_tree::parse_roster_file(const QString &rfile_str) {
    //tools::roster_parser rp(rfile_str, race);
    //auto units = rp.parse();
    //for (auto&& x : units) roster[x.name] = std::make_shared<base_unit>(x);
    tools::experimental_parser ep(rfile_str, race);
    auto units = ep.parse();
    for (auto&& x : units) roster[x.name] = std::make_shared<base_unit>(x);
}

void selection_tree::parse_item_file(const QString& ifile_str) {
    tools::item_parser ip(ifile_str);
    auto items = ip.parse();
    for (auto&& item : items) magic_items[item.name] = item;
    std::shared_ptr<std::unordered_map<std::string, magic_item>> sp_items
        = std::make_shared<std::unordered_map<std::string, magic_item>>(magic_items);
    for (auto& x : roster) x.second->magic_items = sp_items;
}

std::vector<std::shared_ptr<base_unit>> selection_tree::all_of(armies::UnitType ut) const noexcept {
    std::vector<std::shared_ptr<base_unit>> v;
    for (const auto& x : roster) {
        if (x.second->unit_type == ut) v.push_back(x.second);
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
