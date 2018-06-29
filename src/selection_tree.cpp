#include "selection_tree.h"

selection_tree::selection_tree(armies::Faction faction) : race(faction) {
    std::string roster_file;
    std::string item_file;
    switch (race) {
    case armies::Faction::EMPIRE:
        roster_file = "empire_roster.ros";
        break;
    case armies::Faction::BRETONNIA:
        roster_file = "bretonnia_roster.ros";
        break;
    case armies::Faction::DWARFS:
        roster_file = "dwarfs_roster.ros";
        break;
    case armies::Faction::HIGH_ELVES:
        roster_file = "high_elves_roster.ros";
        break;
    case armies::Faction::WOOD_ELVES:
        roster_file = "wood_elves_roster.ros";
        break;
    case armies::Faction::DARK_ELVES:
        roster_file = "dark_elves_roster.ros";
        break;
    case armies::Faction::WARRIORS_OF_CHAOS:
        roster_file = "woc_roster.ros";
        break;
    case armies::Faction::DAEMONS_OF_CHAOS:
        roster_file = "doc_roster.ros";
        break;
    case armies::Faction::BEASTMEN:
        roster_file = "beastmen_roster.ros";
        break;
    case armies::Faction::CHAOS_DWARFS:
        roster_file = "chaos_dwarfs_roster.ros";
        break;
    case armies::Faction::VAMPIRE_COUNTS:
        roster_file = "vampire_counts_roster.ros";
        break;
    case armies::Faction::TOMB_KINGS:
        roster_file = "tomb_kings_roster.ros";
        break;
    case armies::Faction::OGRE_KINGDOMS:
        roster_file = "ogre_kingdoms_roster.ros";
        break;
    case armies::Faction::SKAVEN:
        roster_file = "skaven_roster.ros";
        item_file = "skaven.mag";
        break;
    case armies::Faction::ORCS_AND_GOBLINS:
        roster_file = "orcs_goblins_roster.ros";
        break;
    default:
        roster_file = "empire_roster.ros";
        break;
    }
    parse_roster_file("../rosters/" + roster_file);
    parse_item_file("../magic_items/" + item_file);
}

void selection_tree::change_selection(const std::string& name) {
    current_selection = std::make_shared<unit>(roster[name]);
}

std::shared_ptr<unit>& selection_tree::selected() {
    return current_selection;
}


void selection_tree::add_unit_to_army_list(army_list& list) {
    if (current_selection) list.add_unit(current_selection);
}

void selection_tree::parse_roster_file(const std::string& roster_file) {
    tools::roster_parser rp(roster_file);
    auto units = rp.parse();
    for (auto&& unit : units) roster[unit.get_name()] = unit;
}

void selection_tree::parse_item_file(const std::string& item_file) {
    tools::item_parser ip(item_file);
    auto items = ip.parse();
    for (auto&& item : items) magic_items[item.name] = item;
    auto sp_items = std::make_shared<std::unordered_map<std::string, magic_item>>(magic_items);
    for (auto& key : roster)
        key.second.pass_magic_items_handle(sp_items);
}

