#ifndef HTML_TABLE_UNIT_ROW_H
#define HTML_TABLE_UNIT_ROW_H

#include "enums.h"
#include "units/unit.h"
#include "units/mage_character_unit.h"
#include "units/mixed_unit.h"
#include "tools.h"

#include <memory>
#include <stdexcept>
#include <string>

class html_table_unit_row {
private:
    const std::string tr_start = "<tr>";
    const std::string tr_end = "</tr>";
    const std::string td_start = "<td>";
    const std::string td_end = "</td>";
    const std::string tab = "&nbsp;&nbsp;&nbsp;&nbsp;";
    const std::string newline = "<br/>";
    std::shared_ptr<unit> u;
    std::string urow;

    std::string row(const std::string& s, std::string args="");
    std::string row_entry(const std::string& s, std::string args="");
    std::string bold(const std::string& s);
    std::string characteristics_table(
        const std::vector<std::string>& chars,
        const std::vector<std::string>& champ_chars,
        const std::vector<std::string>& mount_chars
    );
    std::string mixed_characteristics_table(
        const std::vector<std::string>& slave_chars,
        const std::vector<std::string>& slave_champ_chars,
        const std::vector<std::string>& master_chars,
        const std::vector<std::string>& master_champ_chars
    );

    void make_character_row();
    void make_normal_unit_row();
    void make_mixed_unit_row();
public:
    html_table_unit_row(const std::shared_ptr<unit>& u);
    ~html_table_unit_row() = default;

    const std::string& to_string() const noexcept;
};

#endif // !HTML_TABLE_UNIT_ROW_H
