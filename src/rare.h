#include "unit.h"
#include <cstddef>
#include <string>

class rare : public unit {
private:
    //
public:
    rare(const std::string& name,
         std::size_t points,
         std::size_t size,
         std::size_t min_size);
    ~rare();
};
