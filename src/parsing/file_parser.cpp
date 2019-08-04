#include "file_parser.h"

namespace tools {

    file_parser::file_parser(const QString& rfile) : f(rfile) {
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        if (f.size() > static_cast<qint64>(50e6)) {
            throw std::runtime_error("");
        }
        QTextStream in(&f);
        std::string content = in.readAll().toStdString();
        ss = std::stringstream(content);
        cache();
        filename = rfile.toStdString();
        curr_block = 0U;
    }

    file_parser::~file_parser() { f.close(); }

    void file_parser::cache() {
        std::string s;
        while (ss) {
            streampos.push_back(ss.tellg());
            std::getline(ss, s);
        }
    }

    void file_parser::navigate_to_line(std::size_t n) {
        ss.clear();
        ss.seekg(streampos[n]);
    }

    std::string file_parser::read_line(std::size_t n, bool trim) {
        navigate_to_line(n);
        std::string s;
        std::getline(ss, s);
        return trim ? tools::trim(s) : s;
    }

    void file_parser::find_blocks(std::size_t start) {
        for (std::size_t i = start; i < streampos.size(); ++i) {
            auto line = read_line(i, false);
            if (line.empty() || tools::starts_with(line, '#')) continue;
            if (!(tools::starts_with(line, ' ') || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

    std::unordered_multimap<
        RestrictionField,
        std::any
    > file_parser::parse_restrictions(const std::string& s, std::string from, std::vector<char> delim_seq) {
        (void)from;
        if (delim_seq.size() != 3)
            throw std::runtime_error("Deliminator sequence for restriction parsing must be size 3.");
        std::unordered_multimap<
            RestrictionField,
            std::any
        > restrictions;
        auto restr = tools::split(s, delim_seq[0]);
        for (const auto& r : restr) {
            auto field_value = tools::split(r, delim_seq[1]);
            auto search_restr_field = enum_convert::STRING_TO_RESTRICTION.find(field_value[0]);
            if (search_restr_field == std::end(enum_convert::STRING_TO_RESTRICTION)) {
                /*std::string msg = "Error parsing + " + enum_convert::FACTION_TO_STRING.at(faction)
                        + " roster - unit: " + read_line(blocks[curr_block])
                        + " has an invalid argument " + from + " with Restrictions: " + field_value[0];*/
                std::string msg = "restriction parsing error: " + s;
                throw std::runtime_error(msg);
            }
            switch (search_restr_field->second) {
            case RestrictionField::WEAPON:
            case RestrictionField::ARMOUR:
            case RestrictionField::TALISMAN:
            case RestrictionField::ENCHANTED:
            case RestrictionField::ARCANE:
            case RestrictionField::MC_EXTRA:
            case RestrictionField::OTHER:
            case RestrictionField::ARMY_CONTAINS:
            case RestrictionField::LORE:
            {
                auto values = tools::split(field_value[1], delim_seq[2]);
                restrictions.emplace(std::make_pair(search_restr_field->second, values));
                break;
            }
            case RestrictionField::BANNER:
            case RestrictionField::MOUNT:
            case RestrictionField::OCO_EXTRA:
                restrictions.emplace(std::make_pair(search_restr_field->second, field_value[1]));
                break;
            case RestrictionField::LIMIT:
                restrictions.emplace(std::make_pair(search_restr_field->second,
                                                    static_cast<unsigned int>(std::stoi(field_value[1]))));
                break;
            case RestrictionField::ITEMTYPE:
            {
                auto values = tools::split(field_value[1], delim_seq[2]);
                std::vector<ItemType> item_types;
                for (auto v : values) {
                    auto search_it = enum_convert::STRING_TO_ITEM_TYPE.find(v);
                    if (search_it == std::end(enum_convert::STRING_TO_ITEM_TYPE)) {
                        std::string msg = "restriction parsing error: " + v;
                        throw std::runtime_error(msg);
                    }
                    item_types.push_back(search_it->second);
                }
                restrictions.emplace(std::make_pair(search_restr_field->second, item_types));
                break;
            }
            case RestrictionField::SUBITEMTYPE:
            {
                if (!restrictions.count(RestrictionField::ITEMTYPE)) {
                    std::string msg = "restriction parsing error - must contain ItemType"
                                      " for SubItemType to be used.";
                    throw std::runtime_error(msg);
                }
                if (restrictions.count(RestrictionField::ITEMTYPE) > 1) {
                    std::string msg = "restriction parsing error - must contain only 1 ItemType argument"
                                      " for SubItemType to be used.";
                    throw std::runtime_error(msg);
                }
                switch (std::any_cast<std::vector<ItemType>>(restrictions.find(RestrictionField::ITEMTYPE)->second)[0]) {
                case ItemType::WEAPON:
                {
                    auto search_wt = enum_convert::STRING_TO_WEAPON_TYPE.find(field_value[1]);
                    if (search_wt == std::end(enum_convert::STRING_TO_WEAPON_TYPE)) {
                        std::string msg = "restriction parsing error: " + s;
                        throw std::runtime_error(msg);
                    }
                    restrictions.emplace(std::make_pair(search_restr_field->second, search_wt->second));
                    break;
                }
                case ItemType::ARMOUR:
                {
                    auto search_at = enum_convert::STRING_TO_ARMOUR_TYPE.find(field_value[1]);
                    if (search_at == std::end(enum_convert::STRING_TO_ARMOUR_TYPE)) {
                        std::string msg = "restriction parsing error: " + s;
                        throw std::runtime_error(msg);
                    }
                    restrictions.emplace(std::make_pair(search_restr_field->second, search_at->second));
                    break;
                }
                default: break;
                }
                break;
            }
            }
        }
        return restrictions;
    }

}
