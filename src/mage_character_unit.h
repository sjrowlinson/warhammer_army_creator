#ifndef MAGE_CHARACTER_UNIT_H
#define MAGE_CHARACTER_UNIT_H

#include "enums.h"
#include "base_unit.h"
#include "base_mage_character_unit.h"
#include "character_unit.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

class mage_character_unit : public character_unit {
private:
    std::pair<short, double> level_;
    std::vector<std::string> lores_;

    std::string restriction_check(
        const std::unordered_multimap<RestrictionField, std::any>& restrictions,
        const std::string& item_name
    ) const override;
public:
    const std::shared_ptr<base_mage_character_unit> handle;

    explicit mage_character_unit(const std::shared_ptr<base_unit>& base, army_list* army_handle);
    mage_character_unit(const mage_character_unit& other);
    ~mage_character_unit() override = default;

    bool is_mage() const noexcept override;

    short level() const noexcept;
    void change_level(short lvl);
    void reset_level();

    const std::vector<std::string>& lores() const noexcept;
    void pick_lore(const std::string& lore);
    void remove_lore(const std::string& lore);

    virtual std::vector<std::string> clear() override;
};

#endif // !MAGE_CHARACTER_UNIT_H
