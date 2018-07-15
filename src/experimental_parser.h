#ifndef EXPERIMENTAL_PARSER_H
#define EXPERIMENTAL_PARSER_H

#include "army_maps.h"
#include "base_unit.h"
#include "tools.h"

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace tools {

    class experimental_parser {
        struct equipment {
            std::unordered_map<WeaponType, std::string> weapons;
            std::unordered_map<ArmourType, std::string> armour;
            std::vector<std::string> talismans;
            std::vector<std::string> arcane;
            std::vector<std::string> enchanted;
            std::vector<std::string> banners;

            equipment() 
                : weapons(), armour(), talismans(), arcane(), enchanted(), banners() {}
        };
    private:
        QFile f;
        QTextStream qts;
        std::vector<qint64> streampos;
        std::vector<std::size_t> blocks;
        armies::Faction faction;

        void cache();
        void count_units();
        void navigate_to_line(std::size_t n);
        std::string read_line(std::size_t n, bool trim=true);

        // common parsing
        std::pair<std::size_t, std::size_t> parse_minmax_size(std::string s);
        std::unordered_map<
            CommandGroup,
            std::pair<std::string, double>
        > parse_command(std::string s);
        equipment parse_equipment(std::string s);
        std::unordered_map<
            std::string,
            std::vector<std::pair<WeaponType, double>>
        > parse_optional_weapons(std::string s);
        std::unordered_map<
            std::string,
            std::vector<std::pair<ArmourType, double>>
        > parse_optional_armour(std::string s);


        // specialised parsing
        base_unit parse_melee_character(std::size_t n, armies::UnitType ut);
        base_unit parse_mage_character(std::size_t n, armies::UnitType ut);
    public:
        explicit experimental_parser(const QString& rfile, armies::Faction faction);
        ~experimental_parser();

        std::vector<base_unit> parse();
    };

}

#endif // !EXPERIMENTAL_PARSER_H
