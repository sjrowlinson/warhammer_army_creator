#ifndef MOUNTS_PARSER_H
#define MOUNTS_PARSER_H

#include "../enums.h"
#include "file_parser.h"
#include "../mount.h"
#include "../tools.h"

#include <QString>

#include <functional>
#include <unordered_map>
#include <vector>

namespace tools {

    class mounts_parser : public file_parser {
        struct tmp_parse_obj {
            UnitCategory category;
            std::vector<std::string> characteristics;
            std::vector<std::string> special_rules;
        };

    private:
        tmp_parse_obj tpm;
        std::unordered_map<
            std::string,
            std::function<std::size_t(const std::string&)>
        > parsing_functions;

        void register_bindings();
        mount parse_mount(std::size_t block_pos);

        std::size_t parse_mount_category(const std::string& s);
        std::size_t parse_mount_characteristics(const std::string& s);
        std::size_t parse_mount_special_rules(const std::string& s);
    public:
        explicit mounts_parser(const QString& rfile);
        ~mounts_parser() = default;

        std::vector<mount> parse();
    };

}

#endif // !MOUNTS_PARSER_H
