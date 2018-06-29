#include "selection_tree.h"

selection_tree::selection_tree(armies::Faction faction) : race(faction) {
    std::string filename;
    switch (race) {
    case armies::Faction::EMPIRE:
        filename = "empire_roster.ros";
        break;
    case armies::Faction::BRETONNIA:
        filename = "bretonnia_roster.ros";
        break;
    case armies::Faction::DWARFS:
        filename = "dwarfs_roster.ros";
        break;
    case armies::Faction::HIGH_ELVES:
        filename = "high_elves_roster.ros";
        break;
    case armies::Faction::WOOD_ELVES:
        filename = "wood_elves_roster.ros";
        break;
    case armies::Faction::DARK_ELVES:
        filename = "dark_elves_roster.ros";
        break;
    case armies::Faction::WARRIORS_OF_CHAOS:
        filename = "woc_roster.ros";
        break;
    case armies::Faction::DAEMONS_OF_CHAOS:
        filename = "doc_roster.ros";
        break;
    case armies::Faction::BEASTMEN:
        filename = "beastmen_roster.ros";
        break;
    case armies::Faction::CHAOS_DWARFS:
        filename = "chaos_dwarfs_roster.ros";
        break;
    case armies::Faction::VAMPIRE_COUNTS:
        filename = "vampire_counts_roster.ros";
        break;
    case armies::Faction::TOMB_KINGS:
        filename = "tomb_kings_roster.ros";
        break;
    case armies::Faction::OGRE_KINGDOMS:
        filename = "ogre_kingdoms_roster.ros";
        break;
    case armies::Faction::SKAVEN:
        filename = "skaven_roster.ros";
        break;
    case armies::Faction::ORCS_AND_GOBLINS:
        filename = "orcs_goblins_roster.ros";
        break;
    default:
        filename = "empire_roster.ros";
        break;
    }
    parse_roster_file("../rosters/" + filename);
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

void selection_tree::parse_roster_file(const std::string& filename) {
    tools::roster_parser rp(filename);
    auto units = rp.parse();
    for (auto&& unit : units) roster[unit.get_name()] = unit;
}
