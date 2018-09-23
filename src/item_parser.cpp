#include "item_parser.h"

namespace tools {

    item_parser::item_parser(const QString& ifile_str, ItemClass ic)
        : file_parser(ifile_str), item_class(ic) {
        find_blocks(1U);
    }

    std::string item_parser::name() {
        return read_line(0);
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
            item item;
            item.item_class = item_class;
            item.item_type = mit;
            item.weapon_type = wt;
            item.armour_type = at;
            item.name = name;
            item.points = points;
            item.description = descr;
            if (allowed != "None") {
                std::vector<std::string> vec = tools::split(allowed, ',');
                for (const auto& x : vec) item.allowed_units.insert(x);
            }
            items.push_back(item);
        }
        return items;
    }

}
