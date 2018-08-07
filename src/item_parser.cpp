#include "item_parser.h"

namespace tools {

    item_parser::item_parser(const QString& ifile_str, ItemClass ic)
        : item_file(ifile_str), item_class(ic) {
        item_file.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&item_file);
        std::string content = in.readAll().toStdString();
        ss = std::stringstream(content);
        cache_streampos();
    }

    item_parser::~item_parser() {
        item_file.close();
    }

    void item_parser::cache_streampos() {
        std::string s;
        while (ss) {
            streampos.push_back(ss.tellg());
            std::getline(ss, s);
        }
    }

    void item_parser::count_items() {
        for (std::size_t i = 0U; i < streampos.size(); ++i) {
            std::string line = read_line(i, false);
            if (tools::starts_with(line, '#') || line.empty()) continue;
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    void item_parser::navigate_to_line(std::size_t n) {
        ss.clear();
        ss.seekg(streampos[n]);
    }

    std::string item_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(ss, s);
        if (trim) return tools::remove_leading_whitespaces(s);
        return s;
    }

    std::vector<item> item_parser::parse() {
        std::unordered_map<
            std::string,
            ItemType
        > map_to_item = {
            {"Weapon", ItemType::WEAPON},
            {"Armour", ItemType::ARMOUR},
            {"Talisman", ItemType::TALISMAN},
            {"Arcane", ItemType::ARCANE},
            {"Enchanted", ItemType::ENCHANTED},
            {"Banner", ItemType::BANNER},
            {"Other", ItemType::OTHER}
        };
        std::unordered_map<
            std::string,
            WeaponType
        > map_to_weapon = {
            {"Melee", WeaponType::MELEE},
            {"Ballistic", WeaponType::BALLISTIC}
        };
        std::unordered_map<
            std::string,
            ArmourType
        > map_to_armour = {
            {"Armour", ArmourType::ARMOUR},
            {"Shield", ArmourType::SHIELD},
            {"Helmet", ArmourType::HELMET}
        };
        count_items();
        std::vector<item> items; items.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string name = read_line(blocks[i]);
            std::vector<std::string> mits = tools::split(read_line(blocks[i] + 1), ',');
            for (auto& x : mits) {
                tools::remove_leading_whitespaces(x);
                tools::remove_trailing_whitespaces(x);
            }
            ItemType mit;
            WeaponType wt = WeaponType::NONE;
            ArmourType at = ArmourType::NONE;
            if (mits[0] == "Weapon") {
                mit = ItemType::WEAPON;
                wt = map_to_weapon[mits[1]];
            }
            else if (mits[0] == "Armour") {
                mit = ItemType::ARMOUR;
                at = map_to_armour[mits[1]];
            }
            else mit = map_to_item[mits[0]];
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
