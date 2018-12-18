#ifndef ROSTER_PARSER_H
#define ROSTER_PARSER_H

#include "base_unit.h"
#include "base_character_unit.h"
#include "base_mage_character_unit.h"
#include "base_melee_character_unit.h"
#include "base_mixed_unit.h"
#include "base_normal_unit.h"
#include "enums.h"
#include "file_parser.h"
#include "tools.h"

#include <algorithm>
#include <functional>
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
        struct tmp_parse_obj {
            UnitType unit_type;
            UnitClass unit_class;
            std::string mount;
            double points;
            std::pair<std::size_t, std::size_t> size;
            short mage_level;
            std::unordered_map<short, double> mage_upgrades;
            std::vector<std::string> mage_lores;
            std::vector<std::string> characteristics;
            std::vector<std::string> champ_characteristics;
            std::vector<std::string> special_rules;
            std::vector<std::string> champ_special_rules;
            equipment eq;
            equipment champ_eq;
            options opt;
            options champ_opt;
            std::unordered_map<
                CommandGroup,
                std::pair<std::string, double>
            > command;
            double mb_budget;
            double mi_budget;
            double champ_mi_budget;
            double fi_budget;
            double champ_fi_budget;
            double ti_budget;
            double champ_ti_budget;
            bool unique;
        };

    private:
        Faction faction;
        tmp_parse_obj tpo;
        std::unordered_map<
            std::string,
            std::function<void(const std::string&, bool, bool)>
        > parsing_functions;

        // TESTING

        void register_bindings();

        std::unique_ptr<base_unit> parse_unit(std::size_t block_pos, BaseUnitType but);

        void parse_unit_type(const std::string& s, bool champion, bool master);
        void parse_unit_category(const std::string& s, bool champion, bool master);
        void parse_unit_mount(const std::string& s, bool champion, bool master);
        void parse_unit_points(const std::string& s, bool champion, bool master);
        void parse_unit_size(const std::string& s, bool champion, bool master);
        void parse_unit_mage_level(const std::string& s, bool champion, bool master);
        void parse_unit_mage_upgrades(const std::string& s, bool champion, bool master);
        void parse_unit_characteristics(const std::string& s, bool champion, bool master);
        void parse_unit_special_rules(const std::string& s, bool champion, bool master);
        void parse_unit_equipment(const std::string& s, bool champion, bool master);
        void parse_unit_optional_weapons(const std::string& s, bool champion, bool master);
        void parse_unit_optional_armour(const std::string& s, bool champion, bool master);
        void parse_unit_optional_mounts(const std::string& s, bool champion, bool master);
        void parse_unit_optional_oco_extras(const std::string& s, bool champion, bool master);
        void parse_unit_optional_mc_extras(const std::string& s, bool champion, bool master);
        void parse_unit_optional_command(const std::string& s, bool champion, bool master);
        void parse_unit_banner_budget(const std::string& s, bool champion, bool master);
        void parse_unit_mi_budget(const std::string& s, bool champion, bool master);
        void parse_unit_fi_budget(const std::string& s, bool champion, bool master);
        void parse_unit_ti_budget(const std::string& s, bool champion, bool master);
        void parse_unit_uniqueness(const std::string& s, bool champion, bool master);

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
            double
        > parse_optional_mounts(std::string s);
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        > parse_optional_extras(std::string s);
        std::tuple<
            double,
            std::size_t,
            ItemClass,
            ItemType
        > parse_item_budget(const std::string& s);

        // specialised parsing
        base_melee_character_unit parse_melee_character(std::size_t n, UnitType ut);
        base_mage_character_unit parse_mage_character(std::size_t n, UnitType ut);
        base_normal_unit parse_normal_unit(
                            std::size_t n,
                            UnitType ut,
                            UnitClass category,
                            std::string name_="",
                            std::size_t offset=0
                         );
        base_normal_unit parse_minimal_normal_unit(
                            std::size_t n,
                            UnitType ut,
                            UnitClass category
                         );
        base_normal_unit parse_warmachine(
                            std::size_t n,
                            UnitType ut,
                            UnitClass category
                         );
        base_mixed_unit parse_mixed_unit(std::size_t n, UnitType ut, UnitClass category);
        // unique parsing
        base_normal_unit parse_gyro_unit(
            std::size_t n,
            UnitType ut,
            UnitClass category
        );
    public:
        explicit roster_parser(const QString& rfile, Faction faction);
        ~roster_parser() = default;

        std::vector<std::shared_ptr<base_unit>> parse();
        std::vector<std::shared_ptr<base_unit>> parse_();
    };

}

#endif // !ROSTER_PARSER_H
