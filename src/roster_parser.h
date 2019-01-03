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
            UnitCategory unit_category;
            std::string mount;
            double points = 0.0;
            std::pair<std::size_t, std::size_t> size = {1U, 1U};
            short mage_level = 0;
            std::unordered_map<short, double> mage_upgrades;
            std::vector<std::string> mage_lores;
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

        std::size_t parse_unit_type(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_category(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mount(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_points(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_size(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_level(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_upgrades(const std::string& s, bool champion, bool master);
        std::size_t parse_unit_mage_lores(const std::string& s, bool champion, bool master);
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

        std::pair<std::string, std::size_t> multiline_state_handler(const std::string& s);

        /*std::tuple<
            double,
            std::size_t,
            ItemCategory,
            ItemType
        > parse_item_budget(const std::string& s);*/
    public:
        explicit roster_parser(const QString& rfile, Faction faction);
        ~roster_parser() = default;

        std::vector<std::shared_ptr<base_unit>> parse();
    };

}

#endif // !ROSTER_PARSER_H
