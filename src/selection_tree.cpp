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
    // parse the file into this->roster
    parse_roster_file(filename);
}

selection_tree::~selection_tree() {}

void selection_tree::change_selection(const std::string& name) {
    switch (roster[name].get_type()) {
    case armies::UnitType::LORD:
        current_selection = std::make_shared<lord>(dynamic_cast<lord&>(roster[name]));
        break;
    case armies::UnitType::HERO:
        current_selection = std::make_shared<hero>(dynamic_cast<hero&>(roster[name]));
        break;
    case armies::UnitType::CORE:
        current_selection = std::make_shared<core>(dynamic_cast<core&>(roster[name]));
        break;
    case armies::UnitType::SPECIAL:
        current_selection = std::make_shared<special>(dynamic_cast<special&>(roster[name]));
        break;
    case armies::UnitType::RARE:
        current_selection = std::make_shared<rare>(dynamic_cast<rare&>(roster[name]));
        break;
    }
}

void selection_tree::add_unit_to_army_list(army_list& list) {
    list.add_unit(current_selection);
}

std::vector<
    std::pair<std::string, std::string>
> selection_tree::parse_table(const std::string& table) {
    std::vector<std::string> v = tools::split(table, ',');
    for (auto& s : v) tools::remove_leading_whitespaces(s);
    std::vector<std::pair<std::string, std::string>> pv;
    pv.resize(std::ceil(v.size()/2U));
    for (std::size_t i = 0U; i < pv.size(); ++i) {
        std::string s = v[2*i];
        (i % 2U) ? pv[i].second = s : pv[i].first = s;
    }
    return pv;
}

void selection_tree::parse_stat_table(unit& temp, const std::string& table) {
    std::vector<short> stat_vec = tools::split_stos(table, ',');
    std::array<short, 9U> stats;
    for (std::size_t i = 0U; i < 9U; ++i) stats[i] = stat_vec[i];
    temp.set_stat_table(std::move(stats));
}

void selection_tree::parse_equipment_table(unit& temp, const std::string& table) {
    auto equipment = parse_table(table);
    temp.set_equipment(std::move(equipment));
}

void selection_tree::parse_special_rules_table(unit& temp, const std::string& table) {
    auto special_rules = parse_table(table);
    temp.set_special_rules(std::move(special_rules));
}

void selection_tree::parse_options_table(unit& temp, const std::string& table) {
    // TODO: implement parse_options_table method
}

void selection_tree::parse_roster_file(const std::string& filename) {
    tools::file_reader fr("../rosters/" + filename);
    armies::Faction race;
    for (std::size_t i = 0U; i < fr.lines(); ++i) {
        std::string line = fr.read_line(i);
        if (tools::starts_with(line, '#')) continue;
        if (tools::starts_with(line, ':'))
            race = armies::s_map_string_faction[line.substr(1)];
        else {
            std::vector<std::string> split_line = tools::split(line, ';');
            if (split_line.empty()) continue;
            for (auto& s : split_line) tools::remove_leading_whitespaces(s);
            std::string unit_name = split_line[0];
            armies::UnitType unit_type = armies::s_map_string_unit_type[split_line[1]];
            std::size_t unit_pts = static_cast<std::size_t>(std::stoul(split_line[2]));
            std::size_t unit_min_size = static_cast<std::size_t>(std::stoul(split_line[3]));
            unit tmp;
            switch (unit_type) {
            case armies::UnitType::LORD:
                tmp = lord(race, unit_name, unit_pts, false);
                break;
            case armies::UnitType::HERO:
                tmp = hero(race, unit_name, unit_pts, false);
                break;
            case armies::UnitType::CORE:
                tmp = core(race, unit_name, unit_pts, 0U, unit_min_size);
                break;
            case armies::UnitType::SPECIAL:
                tmp = special(race, unit_name, unit_pts, 0U, unit_min_size);
                break;
            case armies::UnitType::RARE:
                tmp = rare(race, unit_name, unit_pts, 0U, unit_min_size);
                break;
            default:
                break;
            }
            parse_stat_table(tmp, split_line[4]);
            parse_equipment_table(tmp, split_line[5]);
            parse_special_rules_table(tmp, split_line[6]);
            parse_options_table(tmp, split_line[7]);
            roster[split_line[0]] = tmp;
        }
    }
}
