#ifndef ROSTER_PARSER_H
#define ROSTER_PARSER_H

#include "../base_units/base_unit.h"
#include "../base_units/base_character_unit.h"
#include "../base_units/base_mage_character_unit.h"
#include "../base_units/base_melee_character_unit.h"
#include "../base_units/base_mixed_unit.h"
#include "../base_units/base_normal_unit.h"
#include "../enums.h"
#include "../tools.h"
#include "file_parser.h"

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
            // COMBINED
            UnitType unit_type;
            UnitCategory unit_category;
            std::string mount;
            double points = 0.0;
            // MASTER
            std::pair<std::size_t, std::size_t> size = {1U, 1U};
            std::string master_name;
            double master_points = 0.0;
            short mage_level = 0;
            short mage_lore_count = 0;
            std::unordered_map<short, double> mage_upgrades;
            std::vector<lore_option> mage_lores;
            std::vector<std::string> characteristics;
            std::vector<std::string> champ_characteristics;
            std::vector<std::string> special_rules;
            std::vector<std::string> champ_special_rules;
            tmp_parse_equipment eq;
            tmp_parse_equipment champ_eq;
            tmp_parse_options opt;
            tmp_parse_options champ_opt;
            std::unordered_map<
                CommandGroup,
                std::pair<std::string, double>
            > command;
            // SLAVE
            std::pair<std::size_t, std::size_t> slave_size = {1U, 1U};
            std::string slave_name;
            double slave_points = 0.0;
            std::vector<std::string> slave_characteristics;
            std::vector<std::string> slave_champ_characteristics;
            std::vector<std::string> slave_special_rules;
            std::vector<std::string> slave_champ_special_rules;
            tmp_parse_equipment slave_eq;
            tmp_parse_equipment slave_champ_eq;
            tmp_parse_options slave_opt;
            tmp_parse_options slave_champ_opt;
            std::unordered_map<
                CommandGroup,
                std::pair<std::string, double>
            > slave_command;
            // OTHER/COMBINED
            double mb_budget = 0.0;
            budget mi_budget = budget();
            budget fi_budget = budget();
            budget ti_budget = budget();
            bool unique = false;
        };

    private:
        Faction faction;
        tmp_parse_obj tpo;
        std::unordered_map<
            std::string,
            std::function<std::size_t(const std::string&, bool, bool)>
        > parsing_functions;
        std::size_t curr_line;
        bool in_multiline_state;

        void register_bindings();

        std::unique_ptr<base_unit> parse_unit(std::size_t block_pos, BaseUnitType but);

        std::size_t parse_unit_mixed_name(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_type(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_category(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mount(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_points(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_size(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_level(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_upgrades(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_lores(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_lore_count(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_characteristics(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_special_rules(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_equipment(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_weapons(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_armour(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_mounts(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_oco_extras(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_mc_extras(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_optional_command(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_banner_budget(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mi_budget(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_fi_budget(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_ti_budget(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_uniqueness(const std::string& s, bool champion, bool master);

        std::size_t parse_unit_extras(const std::string& s, bool champion, bool master, bool oco);
        std::size_t parse_unit_budget(const std::string& s, BudgetType bt);

        std::unordered_map<std::string, extra_option> parse_unit_mount_options(const std::string& s, bool oco);

        std::pair<std::string, std::size_t> multiline_state_handler(const std::string& s);
    public:
        explicit roster_parser(const QString& rfile, Faction faction);
        ~roster_parser() = default;

        std::vector<std::shared_ptr<base_unit>> parse();
    };

}

#endif // !ROSTER_PARSER_H
