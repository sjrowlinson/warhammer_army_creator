#ifndef ROSTER_PARSER_H
#define ROSTER_PARSER_H

#include "army_maps.h"
#include "base_unit.h"
#include "base_character_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "file_parser.h"
#include "tools.h"

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QString>

namespace tools {

    class roster_parser : public file_parser {
    private:
        armies::Faction faction;

        // common parsing
        std::pair<std::size_t, std::size_t> parse_minmax_size(std::string s);
        std::unordered_map<
            CommandGroup,
            std::pair<std::string, double>
        > parse_command(std::string s);
        std::unordered_map<
            short,
            double
        > parse_mage_upgrades(std::string s);
        equipment parse_equipment(std::string s);
        std::unordered_map<
            std::string,
            std::tuple<WeaponType, ItemClass, double, std::vector<std::string>>
        > parse_optional_weapons(std::string s);
        std::unordered_map<
            std::string,
            std::tuple<ArmourType, ItemClass, double, std::vector<std::string>>
        > parse_optional_armour(std::string s);
        std::unordered_map<
            std::string,
            std::pair<armies::UnitClass, double>
        > parse_optional_mounts(std::string s);
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        > parse_optional_extras(std::string s);

        // specialised parsing
        base_melee_character_unit parse_melee_character(std::size_t n, armies::UnitType ut);
        base_mage_character_unit parse_mage_character(std::size_t n, armies::UnitType ut);
        base_normal_unit parse_normal_unit(
                            std::size_t n,
                            armies::UnitType ut,
                            armies::UnitClass category,
                            std::string name_="",
                            std::size_t offset=0
                         );
        base_normal_unit parse_minimal_normal_unit(
                            std::size_t n,
                            armies::UnitType ut,
                            armies::UnitClass category
                         );
        base_normal_unit parse_warmachine(
                            std::size_t n,
                            armies::UnitType ut,
                            armies::UnitClass category
                         );
        base_mixed_unit parse_mixed_unit(std::size_t n, armies::UnitType ut, armies::UnitClass category);
        // unique parsing
        base_normal_unit parse_gyro_unit(
            std::size_t n,
            armies::UnitType ut,
            armies::UnitClass category
        );

    public:
        explicit roster_parser(const QString& rfile, armies::Faction faction);
        ~roster_parser() = default;

        std::vector<std::shared_ptr<base_unit>> parse();
    };

}

#endif // !ROSTER_PARSER_H
