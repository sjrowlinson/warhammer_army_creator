#ifndef ITEM_PARSER_H
#define ITEM_PARSER_H

#include "enums.h"
#include "file_parser.h"
#include "magic_item.h"
#include "tools.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace tools {

    class item_parser : public file_parser {
    private:
        ItemCategory item_class;
        item tpi; // > temporary parsing item
        std::unordered_map<
            std::string,
            std::function<std::size_t(const std::string&)>
        > parsing_functions;

        void register_bindings();
        item parse_item(std::size_t block_pos);

        std::size_t parse_item_category(const std::string& s);
        std::size_t parse_item_type(const std::string& s);
        std::size_t parse_item_points(const std::string& s);
        std::size_t parse_item_description(const std::string& s);
        std::size_t parse_item_allowed_units(const std::string& s);
        std::size_t parse_item_restrictions(const std::string& s);
    public:
        explicit item_parser(const QString& ifile_str, ItemCategory ic);
        ~item_parser() = default;

        std::string name();
        std::vector<item> parse();
    };

}

#endif // !ITEM_PARSER_H
