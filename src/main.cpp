#include "army_list.h"
#include "army_maps.h"
#include "selection_tree.h"
#include "unit.h"
#include <iostream>

int main(void) {
    selection_tree st(armies::Faction::SKAVEN);
    army_list army(2000U);
    st.change_selection("Clanrats");
    st.selected().pick_weapon(ItemType::MUNDANE, "Spears");
    st.selected().change_size(40);
    st.add_unit_to_army_list(army);
    std::cout << army.current_points() << std::endl;
    return 0;
}