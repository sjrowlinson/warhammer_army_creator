#ifndef OPTION_BOX_H
#define OPTION_BOX_H

class ArmyCreator;

#include "army_list.h"
#include "enums.h"
#include "magic_item.h"
#include "option_selector.h"
#include "selection_tree.h"

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QSpinBox>
#include <QString>
#include <QTranslator>

class OptionBox {
private:
    ArmyCreator* creator;
    QGroupBox* box;
    std::shared_ptr<unit> current;
    InTree in_tree;

    // unit size and command group options boxes
    QGroupBox* make_size_command_box();
    QGroupBox* make_command_box();
    void command_box_helper(
        QLayout* layout,
        const std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        >& opt_command,
        const std::unordered_map<
            CommandGroup, std::pair<std::string, double>
        >& command,
        bool master=false
    );
    // weapons option boxes
    std::pair<QGroupBox*, QGroupBox*> make_weapons_boxes();
    QGroupBox* make_weapons_subbox(WeaponType wt, bool champion);
    // armour option boxes
    std::pair<QGroupBox*, QGroupBox*> make_armour_boxes();
    QGroupBox* make_armour_subbox(ArmourType at, bool champion);
    // mount options boxes
    QGroupBox* make_mounts_boxes();
    // mage options box
    std::pair<QGroupBox*, QGroupBox*> make_mage_options_boxes();
    // extras boxes
    std::pair<QGroupBox*, QGroupBox*> make_extras_boxes();
    QGroupBox* make_oco_extras_subbox(bool champion);
    QGroupBox* make_mc_extras_subbox(bool champion);
public:
    explicit OptionBox(ArmyCreator* creator_, QGroupBox* box_);
    ~OptionBox() = default;

    void reset(const std::shared_ptr<unit>& current_, InTree in_tree_);

    bool reinitialise();
    void clear();
};

#endif // !OPTION_BOX_H
