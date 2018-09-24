#ifndef MOUNTS_PARSER_H
#define MOUNTS_PARSER_H

#include "enums.h"
#include "base_unit.h"
#include "file_parser.h"
#include "mount.h"
#include "tools.h"

#include <QString>

namespace tools {

    class mounts_parser : public file_parser {
    private:
        std::unordered_map<std::string, double> parse_extras(std::string s);
        std::unordered_map<
            RestrictionField,
            std::vector<std::string>
        > parse_restrictions(std::string s);
    public:
        explicit mounts_parser(const QString& rfile);
        ~mounts_parser() = default;

        std::vector<mount> parse();
    };

}

#endif // !MOUNTS_PARSER_H
