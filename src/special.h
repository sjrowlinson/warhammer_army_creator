#include "unit.h"
#include <cstddef>
#include <string>

class special : public unit {
private:
    //
public:
    special(const std::string& name,
            std::size_t points,
            std::size_t size,
            std::size_t min_size);
    ~special();
};
