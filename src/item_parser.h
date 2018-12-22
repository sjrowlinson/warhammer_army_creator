#ifndef ITEM_PARSER_H
#define ITEM_PARSER_H

#include "enums.h"
#include "file_parser.h"
#include "magic_item.h"
#include "tools.h"

#include <fstream>
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

        std::unordered_map<
            RestrictionField,
            std::vector<std::string>
        > parse_restrictions(const std::string& s);
    public:
        explicit item_parser(const QString& ifile_str, ItemCategory ic);
        ~item_parser() = default;

        std::string name();
        std::vector<item> parse();
    };

}

#endif // !ITEM_PARSER_H
