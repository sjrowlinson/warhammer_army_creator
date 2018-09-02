#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include "tools.h"

#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace tools {
    class file_parser {
    private:
        QFile f;
        std::stringstream ss;
        std::vector<std::stringstream::streampos> streampos;

        void cache();
    protected:
        std::vector<std::size_t> blocks;

        virtual void find_blocks(std::size_t start = 0U) final;
        void navigate_to_line(std::size_t n);
        std::string read_line(std::size_t n, bool trim=true);
    public:
        explicit file_parser(const QString& rfile);
        virtual ~file_parser();
    };
}


#endif // !FILE_PARSER_H
