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
    // ...
    case armies::Faction::SKAVEN:
        filename = "skaven_roster.ros";
        break;
    }
    // parse the file into this->army_map
}

selection_tree::~selection_tree() {}

unit selection_tree::add_unit_to_army_list() {
    
}