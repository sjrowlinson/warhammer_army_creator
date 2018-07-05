#include "item_parser.h"

namespace tools {

    item_parser::item_parser(
        const std::string& name
    ) : fs(name), filename(name), blocks() {
        cache_streampos();
    }

    void item_parser::cache_streampos() {
        std::string s;
        while (fs) {
            streampos.push_back(fs.tellg());
            std::getline(fs, s);
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
        fs.clear();
        fs.seekg(streampos[n]);
    }

    std::string item_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(fs, s);
        if (trim) return tools::remove_leading_whitespaces(s);
        return s;
    }

    std::vector<magic_item> item_parser::parse() {
        std::unordered_map<
            std::string,
            MagicItemType
        > map_to_item = {
            {"Weapon", MagicItemType::WEAPON},
            {"Armour", MagicItemType::ARMOUR},
            {"Talisman", MagicItemType::TALISMAN},
            {"Arcane", MagicItemType::ARCANE},
            {"Enchanted", MagicItemType::ENCHANTED},
            {"Banner", MagicItemType::BANNER}
        };
        count_items();
        std::vector<magic_item> items; items.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            std::string name = read_line(blocks[i]);
            MagicItemType item_type = map_to_item[read_line(blocks[i] + 1)];
            double points = std::stod(read_line(blocks[i] + 2));
            std::string descr = read_line(blocks[i] + 3);
            std::string allowed = read_line(blocks[i] + 4);
            magic_item item;
            item.item_type = item_type;
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