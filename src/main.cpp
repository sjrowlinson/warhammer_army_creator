#include "army_list.h"
#include "army_maps.h"
#include "selection_tree.h"
#include "unit.h"
#include <iostream>

int main(void) {
    selection_tree st(armies::Faction::SKAVEN);
    army_list army(2000U);
    st.change_selection("Warlord");
    st.selected().pick_optional_weapon("The Fellblade");
    st.selected().pick_optional_mount("Rat Ogre Bonebreaker");
    st.add_unit_to_army_list(army);
    std::cout << army.current_points() << std::endl;
    st.change_selection("Plague Priest");
    st.selected().pick_optional_mount("Plague Furnace");
    st.add_unit_to_army_list(army);
    std::cout << army.current_points() << std::endl;
    st.change_selection("Clanrats");
    st.add_unit_to_army_list(army);
    std::cout << army.current_points() << std::endl;
    return 0;
}