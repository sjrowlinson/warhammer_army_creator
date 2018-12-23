#ifndef MAGE_CHARACTER_UNIT_H
#define MAGE_CHARACTER_UNIT_H

#include "enums.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "character_unit.h"

#include <memory>
#include <string>
#include <utility>

class mage_character_unit : public character_unit {
private:
    std::pair<short, double> level_;
    std::pair<std::string, std::pair<ItemCategory, double>> arcane_item_;
public:
    const std::shared_ptr<base_mage_character_unit> handle;

    explicit mage_character_unit(const std::shared_ptr<base_unit>& base);
    mage_character_unit(const mage_character_unit& other);
    ~mage_character_unit() override = default;

    bool is_mage() const noexcept override;

    short level() const noexcept;
    void change_level(short lvl);
    void reset_level();

    const std::pair<std::string, std::pair<ItemCategory, double>>& arcane_item() const noexcept;
    std::string pick_arcane_item(ItemCategory item_class, const std::string& name);
    std::string remove_arcane_item();

    virtual std::vector<std::string> clear() override;

    std::string html_table_row() const override;
};

#endif // !MAGE_CHARACTER_UNIT_H
