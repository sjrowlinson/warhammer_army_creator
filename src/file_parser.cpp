#include "file_parser.h"

namespace tools {

    file_parser::file_parser(const QString& rfile) : f(rfile) {
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f);
        std::string content = in.readAll().toStdString();
        ss = std::stringstream(content);
        cache();
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
            if (!(tools::starts_with(line, "    ") || tools::starts_with(line, '\t')))
                blocks.push_back(i);
        }
    }

}
