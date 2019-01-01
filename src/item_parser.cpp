#include "item_parser.h"

namespace tools {

    item_parser::item_parser(const QString& ifile_str, ItemCategory ic)
        : file_parser(ifile_str), item_class(ic), tpi(), parsing_functions() {
        find_blocks(1U);
    }

    void item_parser::register_bindings() {
        using std::placeholders::_1;
        parsing_functions["CATEGORY"] = std::bind(&item_parser::parse_item_category, this, _1);
        parsing_functions["TYPE"] = std::bind(&item_parser::parse_item_type, this, _1);
        parsing_functions["POINTS"] = std::bind(&item_parser::parse_item_points, this, _1);
        parsing_functions["DESCRIPTION"] = std::bind(&item_parser::parse_item_description, this, _1);
        parsing_functions["ALLOWED_UNITS"] = std::bind(&item_parser::parse_item_allowed_units, this, _1);
        parsing_functions["RESTRICTIONS"] = std::bind(&item_parser::parse_item_category, this, _1);
    }

    std::string item_parser::name() {
        return read_line(0);
    }

    std::vector<item> item_parser::parse() {
        std::vector<item> items;
        items.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            try {
                auto tmp = parse_item(i);
                items.push_back(tmp);
            } catch (const std::runtime_error&) { throw; }
        }
        return items;
    }

    item item_parser::parse_item(std::size_t block_pos) {
        curr_block = block_pos;
        std::string item_name = read_line(blocks[block_pos]);
        tpi = item();
        tpi.name = item_name;
        tpi.item_class = item_class;
        std::size_t i = 1U;
        std::string arg = read_line(blocks[block_pos] + i);
        while (!arg.empty()) {
            auto line = tools::split(arg, '=');
            auto search_parse_fn = parsing_functions.find(line[0]);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Error parsing + " + name() + " item file - "
                    " item: " + item_name + " has an invalid argument: " + line[0];
                throw std::runtime_error(msg);
            }
            try { search_parse_fn->second(tools::trim(line[1])); }
            catch (const std::runtime_error&) { throw; }
            ++i;
            arg = read_line(blocks[block_pos] + i);
        }
        if (!tpi.restrictions.count(RestrictionField::LIMIT))
            tpi.restrictions.emplace(
                std::make_pair(RestrictionField::LIMIT, std::numeric_limits<unsigned int>::max())
            );
        return tpi;
    }

    std::size_t item_parser::parse_item_category(const std::string& s) {
        auto search = enum_convert::STRING_TO_ITEM_TYPE.find(s);
        if (search == std::end(enum_convert::STRING_TO_ITEM_TYPE)) {
            std::string msg = "Error parsing + " + name() + " item file - "
                " item: " + read_line(blocks[curr_block]) + " has an invalid CATEGORY argument: "
                    + s;
            throw std::runtime_error(msg);
        }
        tpi.item_type = search->second;
        return 0U;
    }
    std::size_t item_parser::parse_item_type(const std::string& s) {
        switch (tpi.item_type) {
        case ItemType::WEAPON:
        {
            auto search = enum_convert::STRING_TO_WEAPON_TYPE.find(s);
            if (search == std::end(enum_convert::STRING_TO_WEAPON_TYPE)) {
                std::string msg = "Error parsing + " + name() + " item file - "
                    " item: " + read_line(blocks[curr_block]) + " has an invalid TYPE argument: "
                        + s;
                throw std::runtime_error(msg);
            }
            tpi.weapon_type = search->second;
            break;
        }
        case ItemType::ARMOUR:
        {
            auto search = enum_convert::STRING_TO_ARMOUR_TYPE.find(s);
            if (search == std::end(enum_convert::STRING_TO_ARMOUR_TYPE)) {
                std::string msg = "Error parsing + " + name() + " item file - "
                    " item: " + read_line(blocks[curr_block]) + " has an invalid TYPE argument: "
                        + s;
                throw std::runtime_error(msg);
            }
            tpi.armour_type = search->second;
            break;
        }
        default: break;
        }
        return 0U;
    }
    std::size_t item_parser::parse_item_points(const std::string& s) {
        tpi.points = std::stod(s);
        return 0U;
    }
    std::size_t item_parser::parse_item_description(const std::string& s) {
        tpi.description = s;
        return 0U;
    }
    std::size_t item_parser::parse_item_allowed_units(const std::string& s) {
        auto units = tools::split(s, ',');
        for (const auto& u : units) tpi.allowed_units.insert(u);
        return 0U;
    }
    std::size_t item_parser::parse_item_restrictions(const std::string& s) {
        tpi.restrictions = parse_restrictions(s, "");
    }

}
