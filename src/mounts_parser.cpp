#include "mounts_parser.h"

namespace tools {

    mounts_parser::mounts_parser(const QString& rfile) : file_parser(rfile) {
        find_blocks();
    }



}
