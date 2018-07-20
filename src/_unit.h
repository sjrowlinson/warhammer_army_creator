#ifndef _UNIT_H
#define _UNIT_H

#include "army_maps.h"
#include "base_unit.h"
#include "_base_unit.h"

#include <memory>

class _unit {
private:
    int id_;
    std::shared_ptr<_base_unit> base_;
protected:
    double points_;
public:
};

#endif // !_UNIT_H