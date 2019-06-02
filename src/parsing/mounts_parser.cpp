#include "mounts_parser.h"

namespace tools {

    mounts_parser::mounts_parser(const QString& rfile) : file_parser(rfile) {
        find_blocks();
        register_bindings();
    }

    void mounts_parser::register_bindings() {
        using std::placeholders::_1;
        parsing_functions["CATEGORY"] = std::bind(&mounts_parser::parse_mount_category, this, _1);
        parsing_functions["CHARACTERISTICS"] = std::bind(&mounts_parser::parse_mount_characteristics, this, _1);
        parsing_functions["SPECIAL_RULES"] = std::bind(&mounts_parser::parse_mount_special_rules, this, _1);
    }

    std::vector<mount> mounts_parser::parse() {
        std::vector<mount> mounts;
        mounts.reserve(blocks.size());
        for (std::size_t i = 0U; i < blocks.size(); ++i) {
            try {
                auto tmp = parse_mount(i);
                mounts.push_back(tmp);
            } catch (const std::runtime_error&) { throw; }
        }
        return mounts;
    }

    mount mounts_parser::parse_mount(std::size_t block_pos) {
        curr_block = block_pos;
        std::string mount_name = read_line(blocks[block_pos]);
        tpm = tmp_parse_obj();
        std::size_t i = 1U;
        std::string arg = read_line(blocks[block_pos] + i);
        while (!arg.empty()) {
            auto line = tools::split(arg, '=');
            auto search_parse_fn = parsing_functions.find(line[0]);
            if (search_parse_fn == std::end(parsing_functions)) {
                std::string msg = "Error parsing mounts file - mount: " + mount_name
                        + " has an invalid argument: " + line[0];
                throw std::runtime_error(msg);
            }
            try { search_parse_fn->second(tools::trim(line[1])); }
            catch (const std::runtime_error&) { throw; }
            ++i;
            arg = read_line(blocks[block_pos] + i);
        }
        mount mount_tmp(mount_name, tpm.category, std::move(tpm.characteristics), std::move(tpm.special_rules));
        return mount_tmp;
    }

    std::size_t mounts_parser::parse_mount_category(const std::string& s) {
        auto search = enum_convert::STRING_TO_UNIT_CLASS.find(s);
        if (search == std::end(enum_convert::STRING_TO_UNIT_CLASS)) {
            std::string msg = "Error parsing mounts file - mount: " + read_line(blocks[curr_block])
                    + " has an invalid CATEGORY argument: " + s;
            throw std::runtime_error(msg);
        }
        tpm.category = search->second;
        return 0U;
    }
    std::size_t mounts_parser::parse_mount_characteristics(const std::string& s) {
        tpm.characteristics = tools::split(s, ' ');
        return 0U;
    }
    std::size_t mounts_parser::parse_mount_special_rules(const std::string& s) {
        tpm.special_rules = tools::split(s, ',');
        return 0U;
    }
}
