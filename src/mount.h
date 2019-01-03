#ifndef MOUNT_H
#define MOUNT_H

#include "enums.h"

#include <string>
#include <vector>

class mount {
private:
    std::string name_;
    UnitCategory unit_class_;
    std::vector<std::string> statistics_;
    std::vector<std::string> special_rules_;
public:
    mount();
    explicit mount(
        std::string _name,
        UnitCategory _uc,
        std::vector<std::string>&& _stats,
        std::vector<std::string>&& _sr
    );
    ~mount() = default;

    const std::string& name() const noexcept;
    UnitCategory unit_class() const noexcept;
    const std::vector<std::string>& statistics() const noexcept;
    const std::vector<std::string>& special_rules() const noexcept;
};

#endif // !MOUNT_H
