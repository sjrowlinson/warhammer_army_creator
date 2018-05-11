#include "unit.h"
#include <cstddef>
#include <string>

class hero : public unit {
private:
    bool is_named;
public:
    hero(const std::string& name, std::size_t points, bool is_named);
    ~hero();
    bool special_character() const noexcept;
};
