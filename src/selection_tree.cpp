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
    // parse the file into this->roster
    parse_roster_file(filename);
}

selection_tree::~selection_tree() {}

unit selection_tree::add_unit_to_army_list() {
    return *current_selection;
}

void _parse_stat_table(unit& temp, const std::string& table) {
    std::vector<short> stat_vec = tools::split_stos(table, ',');
    std::array<short, 9U> stats;
    for (std::size_t i = 0U; i < 9U; ++i) stats[i] = stat_vec[i];
    temp.set_stat_table(std::move(stats));
}

// TODO: _parse_equipment_table, _parse_special_rules_table have identical
// procedures, create common function for both

void _parse_equipment_table(unit& temp, const std::string& table) {
    std::vector<std::string> equipment_vec = tools::split(table, ',');
    for (auto& s : equipment_vec) tools::remove_leading_whitespaces(s);
    std::vector<std::pair<std::string, std::string>> equipment;
    equipment.resize(std::ceil(equipment.size()/2U));
    for (std::size_t i = 0U; i < equipment.size(); ++i) {
        std::string s = equipment_vec[2*i];
        (i % 2U) ? equipment[i].second = s : equipment[i].first = s;
    }
    temp.set_equipment(std::move(equipment));
}

void _parse_special_rules_table(unit& temp, const std::string& table) {
    std::vector<std::string> special_rules_vec = tools::split(table, ',');
    for (auto& s : special_rules_vec) tools::remove_leading_whitespaces(s);
    std::vector<std::pair<std::string, std::string>> special_rules;
    special_rules.resize(std::ceil(special_rules_vec.size()/2U));
    for (std::size_t i = 0U; i < special_rules.size(); ++i) {
        std::string s = special_rules_vec[2*i];
        (i % 2U) ? special_rules[i].second = s : special_rules[i].first = s;
    }
    temp.set_special_rules(std::move(special_rules));
}

void selection_tree::parse_roster_file(const std::string& filename) {
    tools::file_reader fr(filename);
    for (std::size_t i = 0U; i < fr.lines(); ++i) {
        std::string line = fr.read_line(i);
        std::vector<std::string> split_line = tools::split(line, ';');
        for (auto& s : split_line) tools::remove_leading_whitespaces(s);
        unit tmp = unit(
            static_cast<armies::Faction>(std::stoi(split_line[1])),
            split_line[0],
            static_cast<std::size_t>(std::stoul(split_line[2])),
            0U,
            static_cast<std::size_t>(std::stoul(split_line[3]))
        );
        _parse_stat_table(tmp, split_line[4]);
        _parse_equipment_table(tmp, split_line[5]);
        _parse_special_rules_table(tmp, split_line[6]);
    }
}