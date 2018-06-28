#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(
        const std::string& name
    ) : fs(name), filename(name), streampos(), blocks(), faction(armies::Faction::NONE) {
        cache_streampos();
    }

    void roster_parser::cache_streampos() {
        std::string s;
        while (fs) {
            streampos.push_back(fs.tellg());
            std::getline(fs, s);
        }
    }

    void roster_parser::count_units() {
        for (std::size_t i = 1U; i < streampos.size(); ++i) {
            std::string line = read_line(i, false);
            if (tools::starts_with(line, '#') || line.empty()) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void roster_parser::navigate_to_line(std::size_t n) {
        fs.clear();
        fs.seekg(streampos[n]);
    }

    std::string roster_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(fs, s);
        if (trim) return tools::remove_leading_whitespaces(s);
        return s;
    }

    std::vector<std::pair<std::string, double>> roster_parser::common_option_parse(std::string s) {
        std::vector<std::pair<std::string, double>> vec;
        if (s == "None" || s.empty()) return vec;
        // first parse to vec of strings
        std::vector<std::string> svec = tools::split(s, ',');
        for (const auto& _s : svec) {
            std::vector<std::string> tmp = tools::split(_s, '[');
            std::string pts_str;
            std::copy_if(
                tmp[1].begin(), tmp[1].end(), pts_str.begin(), [](auto x) { return x != ']'; }
            );
            double pts = std::stod(pts_str);
            vec.push_back(
                std::make_pair(tmp[0].substr(0, tmp[0].size() - 1U), pts)
            );
        }
        return vec;
    }

    std::vector<unit> roster_parser::parse() {
        faction = armies::s_map_string_faction[read_line(0)];
        count_units();
        std::vector<unit> units; units.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string types = read_line(blocks[i]+1);
            std::vector<std::string> types_split = tools::split(types, ',');
            for (auto& s : types_split) tools::remove_leading_whitespaces(s);
            armies::UnitType unit_type = armies::s_map_string_unit_type[types_split[0]];
            switch (unit_type) {
            case armies::UnitType::LORD:
            case armies::UnitType::HERO:
                if (types_split[1] == "Melee") units.push_back(parse_melee_character(i, unit_type));
                else if (types_split[1] == "Mage") parse_mage_character(i, unit_type);
                break;
            case armies::UnitType::CORE:
            case armies::UnitType::SPECIAL:
            case armies::UnitType::RARE:
                if (types_split[1] == "Infantry") parse_infantry(i, unit_type);
                else if (types_split[1] == "Cavalry") parse_cavalry(i, unit_type);
                else if (types_split[1] == "Warbeast") parse_warbeast(i, unit_type);
                else if (types_split[1] == "Monstrous Creature") parse_monstrous_creature(i, unit_type);
                else if (types_split[1] == "Monster") parse_monster(i, unit_type);
                break;
            default: continue;
            }
        }
        return units;
    }

	unit roster_parser::parse_melee_character(std::size_t index, armies::UnitType unit_type) {
        std::string name = read_line(blocks[index]);
        armies::UnitClass category = armies::s_map_string_unit_class[
            read_line(blocks[index] + 2)
        ];
        std::size_t pts_per_model = std::stoul(read_line(blocks[index] + 3));
        std::size_t min_size = std::stoul(read_line(blocks[index] + 4));
        std::vector<short> stats = tools::split_stos(read_line(blocks[index] + 5), ' ');
        std::vector<std::string> equipment = tools::split(read_line(blocks[index] + 6), ',');
        std::vector<std::string> special_rules = tools::split(read_line(blocks[index] + 7), ',');
        std::vector<
            std::pair<std::string, double>
        > optional_weapons = common_option_parse(read_line(blocks[index] + 8));
        std::vector<
            std::pair<std::string, double>
        > optional_armour = common_option_parse(read_line(blocks[index] + 9));
        std::vector<
            std::pair<std::string, double>
        > optional_mounts = common_option_parse(read_line(blocks[index] + 10));
        std::size_t magic_item_allowance = std::stoul(read_line(blocks[index] + 11));
        std::vector<std::string> str_extra_allowance = tools::split(read_line(blocks[index] + 12), ',');
        std::size_t extra_item_allowance = std::stoul(str_extra_allowance[0]);
        std::size_t total_allowance;
        if (str_extra_allowance[1] == "true") total_allowance = magic_item_allowance;
        else total_allowance = magic_item_allowance + extra_item_allowance;
        unit tmp(
            faction, unit_type, category, name,pts_per_model, min_size, min_size
        );
        // extras (faction-specific)
        if (read_line(blocks[index]+13) != "None") {
            std::vector<
                std::pair<std::string, double>
            > extras = common_option_parse(read_line(blocks[index] + 13));
            tmp.init_optional_extras(std::move(extras));
        }
        tmp.init_stat_table(std::move(stats));
        tmp.init_equipment(std::move(equipment));
        tmp.init_special_rules(std::move(special_rules));
        tmp.init_optional_weapons(std::move(optional_weapons));
        tmp.init_optional_armour(std::move(optional_armour));
        tmp.init_optional_mounts(std::move(optional_mounts));
        tmp.init_magic_item_budget(magic_item_allowance);
        tmp.init_extra_item_budget(extra_item_allowance);
        tmp.init_total_item_budget(total_allowance);
        return tmp;
    }

    unit roster_parser::parse_mage_character(std::size_t, armies::UnitType unit_type) {

    }

    unit roster_parser::parse_infantry(std::size_t index, armies::UnitType unit_type) {

    }

	unit roster_parser::parse_cavalry(std::size_t index, armies::UnitType unit_type) {

    }

	unit roster_parser::parse_warbeast(std::size_t index, armies::UnitType unit_type) {

    }

	unit roster_parser::parse_monstrous_creature(std::size_t index, armies::UnitType unit_type) {

    }

	unit roster_parser::parse_monster(std::size_t index, armies::UnitType unit_type) {

    }

}
