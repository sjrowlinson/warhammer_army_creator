#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "enums.h"
#include "tools.h"

#include <any>
#include <fstream>
#include <ios>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace tools {
    class file_parser {
    private:
        QFile f;
        std::stringstream ss;
        std::vector<std::fpos<std::char_traits<char>::state_type>> streampos;

        void cache();
    protected:
        std::string filename;
        std::vector<std::size_t> blocks;
        std::size_t curr_block;

        virtual void find_blocks(std::size_t start = 0U) final;
        void navigate_to_line(std::size_t n);
        std::string read_line(std::size_t n, bool trim=true);

        std::unordered_multimap<
            RestrictionField,
            std::any
        > parse_restrictions(const std::string& s, std::string from);
    public:
        explicit file_parser(const QString& rfile);
        virtual ~file_parser();
    };
}


#endif // !FILE_PARSER_H
