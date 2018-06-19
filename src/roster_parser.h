#ifndef ROSTER_PARSER_H
#define ROSTER_PARSER_H

#include "army_maps.h"
#include "tools.h"
#include "unit.h"

#include <algorithm>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tools {
	class roster_parser {
	private:
		std::fstream fs;
		std::string filename;
		std::vector<std::fstream::streampos> streampos;
		std::vector<std::size_t> blocks;
		armies::Faction faction;

		void cache_streampos();
		void count_units();
		void navigate_to_line(std::size_t n);
		std::string read_line(std::size_t n, bool trim=true);
		unit parse_melee_character(std::size_t index, armies::UnitType unit_type);
		unit parse_mage_character(std::size_t index, armies::UnitType unit_type);
		unit parse_infantry(std::size_t index, armies::UnitType unit_type);
		unit parse_cavalry(std::size_t index, armies::UnitType unit_type);
		unit parse_warbeast(std::size_t index, armies::UnitType unit_type);
		unit parse_monstrous_creature(std::size_t index, armies::UnitType unit_type);
		unit parse_monster(std::size_t index, armies::UnitType unit_type);
		std::vector<std::pair<std::string, double>> common_option_parse(std::string s);
	public:
		explicit roster_parser(
			const std::string& name
		);
		std::size_t units() const noexcept;
		std::vector<unit> parse();
	};

}

#endif // !ROSTER_PARSER_H