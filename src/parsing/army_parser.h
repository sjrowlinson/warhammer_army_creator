#ifndef ARMY_PARSER_H
#define ARMY_PARSER_H

#include "../units/unit.h"
#include "../selection_tree.h"
#include "../tools.h"

#include "file_parser.h"

#include <functional>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QString>

namespace tools {

    class army_parser : public file_parser {
    private:
        std::shared_ptr<selection_tree> selector;
        std::shared_ptr<unit> unit_handle;
        std::set<int> ids;
        std::unordered_map<
            std::string,
            std::function<void(const std::string&, bool, bool)>
        > parsing_functions;
        std::size_t curr_line;

        int general_id_;
        double unit_pts;
        double unit_magic_pts;
        double unit_faction_pts;
        double unit_total_pts;

        std::string cached_mixed_unit_master_size;

        void register_bindings();

        void parse_unit_assigned_name(const std::string& s, bool champion, bool master);
        void parse_unit_id(const std::string& s, bool champion, bool master);
        void parse_unit_melee_weapon(const std::string& s, bool champion, bool master);
        void parse_unit_ranged_weapon(const std::string& s, bool champion, bool master);
        void parse_unit_body_armour(const std::string& s, bool champion, bool master);
        void parse_unit_shield_armour(const std::string& s, bool champion, bool master);
        void parse_unit_helmet_armour(const std::string& s, bool champion, bool master);
        void parse_unit_oco_extra(const std::string& s, bool champion, bool master);
        void parse_unit_mc_extras(const std::string& s, bool champion, bool master);
        void parse_unit_talisman(const std::string& s, bool champion, bool master);
        void parse_unit_enchanted_item(const std::string& s, bool champion, bool master);
        void parse_unit_arcane_item(const std::string& s, bool champion, bool master);
        void parse_unit_item_extras(const std::string& s, bool champion, bool master);
        void parse_unit_banner(const std::string& s, bool champion, bool master);
        void parse_unit_mount(const std::string& s, bool champion, bool master);
        void parse_unit_mount_oco_option(const std::string& s, bool champion, bool master);
        void parse_unit_mount_mc_options(const std::string& s, bool champion, bool master);
        void parse_unit_command(const std::string& s, bool champion, bool master);
        void parse_unit_size(const std::string& s, bool champion, bool master);
        void parse_unit_mage_level(const std::string& s, bool champion, bool master);
        void parse_unit_mage_lores(const std::string& s, bool champion, bool master);

        // these are used purely for sanity checks
        void parse_unit_points(const std::string& s, bool champion, bool master);
        void parse_unit_magic_item_points(const std::string& s, bool champion, bool master);
        void parse_unit_faction_item_points(const std::string& s, bool champion, bool master);
        void parse_unit_total_item_points(const std::string& s, bool champion, bool master);

        void parse_unit(std::size_t block);

        void parse_weapon(const std::string& s, bool champion, bool master, std::string type);
        void parse_armour(const std::string& s, bool champion, bool master, std::string type);

    public:
        explicit army_parser(const QString& file, std::shared_ptr<selection_tree> st);

        Faction parse_faction();
        double parse();
        const std::set<int>& all_ids() const noexcept;
        int general_id() const noexcept;
    };

}

#endif // !ARMY_PARSER_H
