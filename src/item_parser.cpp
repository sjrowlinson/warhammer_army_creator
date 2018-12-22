#include "item_parser.h"

namespace tools {

    item_parser::item_parser(const QString& ifile_str, ItemCategory ic)
        : file_parser(ifile_str), item_class(ic) {
        find_blocks(1U);
    }

    std::string item_parser::name() {
        return read_line(0);
    }

    std::unordered_map<
        RestrictionField,
        std::vector<std::string>
    > item_parser::parse_restrictions(const std::string& s) {
        std::unordered_map<
            RestrictionField,
            std::vector<std::string>
        > um;
        if (s.empty() || s == "None") return um;
        auto restriction = tools::split(s, ';');
        for (const auto& r : restriction) {
            auto split = tools::split(r, ':');
            RestrictionField rf = enum_convert::STRING_TO_RESTRICTION.at(split[0]);
            auto restriction_names = tools::split(split[1], ',');
            for (auto& x : restriction_names) tools::trim(x);
            um[rf] = restriction_names;
        }
        return um;
    }

    std::vector<item> item_parser::parse() {
        std::vector<item> items; items.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string name = read_line(blocks[i]);
            std::vector<std::string> mits = tools::split(read_line(blocks[i] + 1), ',');
            for (auto& x : mits) tools::trim(x);
            ItemType mit;
            WeaponType wt = WeaponType::NONE;
            ArmourType at = ArmourType::NONE;
            if (mits[0] == "Weapon") {
                mit = ItemType::WEAPON;
                wt = enum_convert::STRING_TO_WEAPON_TYPE.at(mits[1]);
            }
            else if (mits[0] == "Armour") {
                mit = ItemType::ARMOUR;
                at = enum_convert::STRING_TO_ARMOUR_TYPE.at(mits[1]);
            }
            else mit = enum_convert::STRING_TO_ITEM_TYPE.at(mits[0]);
            double points = std::stod(read_line(blocks[i] + 2));
            std::string descr = read_line(blocks[i] + 3);
            std::string allowed = read_line(blocks[i] + 4);
            std::string limit = read_line(blocks[i] + 5);
            auto restrs = parse_restrictions(read_line(blocks[i] + 6));
            item itm;
            itm.item_class = item_class;
            itm.item_type = mit;
            itm.weapon_type = wt;
            itm.armour_type = at;
            itm.name = name;
            itm.points = points;
            itm.description = descr;
            if (allowed != "None") {
                std::vector<std::string> vec = tools::split(allowed, ',');
                for (const auto& x : vec) itm.allowed_units.insert(x);
            }
            if (limit != "None") itm.limit = static_cast<unsigned int>(std::stoul(limit));
            else itm.limit = std::numeric_limits<int>::max();
            itm.restrictions = restrs;
            items.push_back(itm);
        }
        return items;
    }

}
