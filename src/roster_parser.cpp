#include "roster_parser.h"

namespace tools {

    roster_parser::roster_parser(
        const std::string& name,
        std::unordered_map<std::string, unit>& roster
    ) : fs(name), filename(name), streampos(), blocks() {
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
            std::string line = read_line(i);
            if (tools::starts_with(line, '#') || line.empty()) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void roster_parser::navigate_to_line(std::size_t n) {
        fs.clear();
        fs.seekg(streampos[n]);
    }

    std::string roster_parser::read_line(std::size_t n) {
        navigate_to_line(n);
        std::string s;
        std::getline(fs, s);
        return s;
    }

    void roster_parser::parse() {
        armies::Faction race = armies::s_map_string_faction[read_line(0)];
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string name = read_line(blocks[i]);
            std::string types = tools::remove_leading_whitespaces(read_line(blocks[i]));
            std::vector<std::string> types_split = tools::split(types, ',');
            armies::UnitType unit_type = armies::s_map_string_unit_type[types_split[0]];
            switch (unit_type) {
            case armies::UnitType::LORD:
                if (types_split[1] == "Melee") {
                    // do melee lord based parsing
                }
                else if (types_split[1] == "Mage") {
                    // do mage lord based parsing
                }
                break;
            case armies::UnitType::HERO:
                if (types_split[1] == "Melee") {
                    // do melee hero based parsing
                }
                else if (types_split[1] == "Mage") {
                    // do mage hero based parsing
                }
                break;
            }
        }
    }

}