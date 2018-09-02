#ifndef MOUNTS_PARSER_H
#define MOUNTS_PARSER_H

#include "army_maps.h"
#include "file_parser.h"
#include "tools.h"

#include <QString>

namespace tools {

    class mounts_parser : public file_parser {
    public:
        explicit mounts_parser(const QString& rfile);
        ~mounts_parser() = default;


    };

}

#endif // !MOUNTS_PARSER_H
