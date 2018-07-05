#include "army_list.h"
#include "army_maps.h"
#include "selection_tree.h"
#include "unit.h"
#include <iostream>

int main(void) {
    army_list army(2000U);
    selection_tree st(armies::Faction::SKAVEN, army);
    // add a melee lord
    st.change_selection("Warlord");
    st.selected().pick_weapon(ItemType::MAGIC, "Weeping Blade");
    st.selected().pick_mount("Rat Ogre Bonebreaker");
    st.add_unit_to_army_list();
    // add a mage hero
    st.change_selection("Plague Priest");
    st.selected().change_mage_level(2);
    st.selected().pick_mount("Plague Furnace");
    st.add_unit_to_army_list();
    // add another hero
    st.change_selection("Warlock Engineer");
    st.selected().change_mage_level(1);
    st.add_unit_to_army_list();
    // add core units
    st.change_selection("Clanrats");
    st.selected().pick_weapon(ItemType::MUNDANE, "Spears");
    st.selected().change_size(40);
    st.selected().add_champion();
    st.selected().add_standard_bearer();
    st.selected().add_musician();
    st.add_unit_to_army_list();
    st.selected().remove_weapon();
    st.add_unit_to_army_list();
    st.change_selection("Stormvermin");
    st.selected().change_size(25);
    st.selected().add_champion();
    st.selected().add_musician();
    st.selected().add_standard_bearer();
    st.add_unit_to_army_list();
    // add special units
    st.change_selection("Plague Monks");
    st.selected().add_champion();
    st.selected().add_musician();
    st.selected().add_standard_bearer();
    st.add_unit_to_army_list();
    std::cout << army.current_points() << std::endl;
    for (auto x : army.get()) {
        std::cout << x->name() << ' ' << x->points() << std::endl;
    }
    return 0;
}