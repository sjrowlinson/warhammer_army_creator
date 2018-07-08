#ifndef ITEM_PARSER_H
#define ITEM_PARSER_H

#include "army_maps.h"
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

    class item_parser {
    private:
        std::stringstream ss;
        std::vector<std::stringstream::streampos> streampos;
        std::vector<std::size_t> blocks;

        void cache_streampos();
        void count_items();
        void navigate_to_line(std::size_t n);
        std::string read_line(std::size_t n, bool trim=true);
    public:
        explicit item_parser(QFile& _file);
        std::vector<magic_item> parse();
    };

}

#endif // !ITEM_PARSER_H
