#include "mounts_parser.h"

namespace tools {

    mounts_parser::mounts_parser(const QString& rfile) : file_parser(rfile) {
        find_blocks();
    }

    std::unordered_map<std::string, double> mounts_parser::parse_extras(std::string s) {
        std::unordered_map<std::string, double> um;
        if (s.empty() || s == "None") return um;
        auto all = tools::split(s, ',');
        for (const auto& entry : all) {
            auto item_pts_pair = tools::parse_item_points(entry);
            um[item_pts_pair.first] = item_pts_pair.second;
        }
        return um;
    }

    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > mounts_parser::parse_restrictions(std::string s) {
        std::unordered_map<
            RestrictionField,
            std::vector<std::string>
        > um;
        if (s.empty() || s == "None") return um;
        auto restriction = tools::split(s, ';');
        for (const auto& r : restriction) {
            auto split = tools::split(r, ':');
            RestrictionField rf = tools::string_to_restriction(split[0]);
            auto restriction_names = tools::split(split[1], ',');
            for (auto& x : restriction_names) tools::trim(x);
            um[rf] = restriction_names;
        }
        return um;
    }

    std::vector<mount> mounts_parser::parse() {
        std::vector<mount> mounts;
        mounts.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string name = read_line(blocks[i]);
            auto unit_class = armies::s_map_string_unit_class[read_line(blocks[i] + 1)];
            auto stats = tools::split(read_line(blocks[i] + 2), ' ');
            auto sr = tools::split(read_line(blocks[i] + 3), ',');
            auto oco_extras = parse_extras(read_line(blocks[i] + 4));
            auto mc_extras = parse_extras(read_line(blocks[i] + 5));
            auto restrictions = parse_restrictions(read_line(blocks[i] + 6));
            mount mnt;
            mnt.name = name;
            mnt.unit_class = unit_class;
            mnt.statistics = stats;
            mnt.special_rules = sr;
            mnt.oco_extras = oco_extras;
            mnt.mc_extras = mc_extras;
            mnt.restrictions = restrictions;
            mounts.push_back(mnt);
        }
        return mounts;
    }

}
