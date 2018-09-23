#include "optionbox.h"
#include "armycreator.h"

OptionBox::OptionBox(ArmyCreator* creator_, QGroupBox* box_) :
    creator(creator_), box(box_), current(), in_tree(InTree::NEITHER) {}

void OptionBox::command_box_helper(QLayout* layout,
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& opt_command,
    const std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    >& command,
    bool master) {
    if (opt_command.count(CommandGroup::MUSICIAN)) {
        auto musician = opt_command.at(CommandGroup::MUSICIAN);
        auto tmp = tools::split(std::to_string(musician.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = musician.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        cb->setObjectName(
            QString((musician.first + "_m_" + ((master) ? "master" : "slave") + "_checkbox").data())
        );
        if (command.count(CommandGroup::MUSICIAN)) cb->setChecked(true);
        creator->connect(cb, SIGNAL(clicked(bool)), creator, SLOT(optional_command_selected()));
        layout->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::STANDARD_BEARER)) {
        auto sb = opt_command.at(CommandGroup::STANDARD_BEARER);
        auto tmp = tools::split(std::to_string(sb.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = sb.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        cb->setObjectName(
            QString((sb.first + "_sb_" + ((master) ? "master" : "slave") + "_checkbox").data())
        );
        if (command.count(CommandGroup::STANDARD_BEARER)) cb->setChecked(true);
        creator->connect(cb, SIGNAL(clicked(bool)), creator, SLOT(optional_command_selected()));
        layout->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::CHAMPION)) {
        auto champ = opt_command.at(CommandGroup::CHAMPION);
        auto tmp = tools::split(std::to_string(champ.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = champ.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(creator->tr(name.data()));
        cb->setObjectName(
            QString((champ.first + "_c_" + ((master) ? "master" : "slave") + "_checkbox").data())
        );
        if (command.count(CommandGroup::CHAMPION)) cb->setChecked(true);
        creator->connect(cb, SIGNAL(clicked(bool)), creator, SLOT(optional_command_selected()));
        layout->addWidget(cb);
    }
}

void OptionBox::clear() {
    auto c = box->children();
    for (auto& x : c) delete x;
    current.reset();
}

void OptionBox::reset(std::shared_ptr<unit> current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

bool OptionBox::reinitialise() {
    if (current == nullptr) return false;
    std::string box_title = current->name() + " Options";
    box->setTitle(creator->tr(box_title.data()));
    QVBoxLayout* vbox = new QVBoxLayout;
    QGroupBox* sc_box = make_size_command_box();
    if (sc_box != nullptr) vbox->addWidget(sc_box);
    auto weapons_boxes = make_weapons_boxes();
    if (weapons_boxes.first != nullptr) vbox->addWidget(weapons_boxes.first);
    if (weapons_boxes.second != nullptr) vbox->addWidget(weapons_boxes.second);
    auto armour_boxes = make_armour_boxes();
    if (armour_boxes.first != nullptr) vbox->addWidget(armour_boxes.first);
    if (armour_boxes.second != nullptr) vbox->addWidget(armour_boxes.second);
    if (current->is_mage()) {
        auto mage_levels_box = make_mage_levels_box();
        if (mage_levels_box != nullptr) vbox->addWidget(mage_levels_box);
    }
    auto mount_box = make_mounts_boxes();
    if (mount_box != nullptr) vbox->addWidget(mount_box);
    auto extras_boxes = make_extras_boxes();
    if (extras_boxes.first != nullptr) vbox->addWidget(extras_boxes.first);
    if (extras_boxes.second != nullptr) vbox->addWidget(extras_boxes.second);
    box->setLayout(vbox);
    return true;
}

QGroupBox* OptionBox::make_size_command_box() {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
        if (current->unit_class() == UnitClass::MONSTER) return nullptr;
        break;
    case BaseUnitType::MIXED:
        break;
    default: return nullptr;
    }
    QGroupBox* sc_groupbox = new QGroupBox(creator->tr("Size and Command Group"));
    QVBoxLayout* sc_box_layout = new QVBoxLayout;
    QHBoxLayout* size_box_layout = new QHBoxLayout;
    // command group
    QGroupBox* command_box = nullptr;
    if (current->base_unit_type() == BaseUnitType::NORMAL) {
        // unit size
        QLabel* label = new QLabel(creator->tr("Unit size"));
        QSpinBox* size_sb = new QSpinBox();
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        size_sb->setMinimum(static_cast<int>(p->min_size()));
        size_sb->setMaximum(
            (p->max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->max_size())
        );
        std::string size_sb_name = p->name() + "_spinbox";
        size_sb->setObjectName(QString(size_sb_name.data()));
        // set value of spinbox to current unit size
        if (in_tree == InTree::ARMY) size_sb->setValue(static_cast<int>(p->size()));
        creator->connect(size_sb, SIGNAL(valueChanged(int)), creator, SLOT(change_unit_size()));
        size_box_layout->addWidget(label);
        size_box_layout->addWidget(size_sb);
        size_box_layout->addStretch(1);
        sc_box_layout->addLayout(size_box_layout);
    }
    else { // BaseUnitType::MIXED
        QLabel* master_label = new QLabel(creator->tr("Number of Packmasters"));
        QSpinBox* master_size_sb = new QSpinBox();
        QLabel* slave_label = new QLabel(creator->tr("Number of %1").arg(current->name().data()));
        QSpinBox* slave_size_sb = new QSpinBox();
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        master_size_sb->setMinimum(static_cast<int>(p->master().min_size()));
        master_size_sb->setMaximum(
            (p->master().max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->master().max_size())
        );
        slave_size_sb->setMinimum(static_cast<int>(p->slave().min_size()));
        slave_size_sb->setMaximum(
            (p->slave().max_size() == std::numeric_limits<std::size_t>::max()) ?
                1000 : static_cast<int>(p->slave().max_size())
        );
        if (in_tree == InTree::ARMY) {
            master_size_sb->setValue(static_cast<int>(p->master().size()));
            slave_size_sb->setValue(static_cast<int>(p->slave().size()));
        }
        std::string master_size_sb_name = p->name() + "_master_spinbox";
        std::string slave_size_sb_name = p->name() + "_slave_spinbox";
        master_size_sb->setObjectName(QString(master_size_sb_name.data()));
        slave_size_sb->setObjectName(QString(slave_size_sb_name.data()));
        creator->connect(master_size_sb, SIGNAL(valueChanged(int)), creator, SLOT(change_unit_size()));
        creator->connect(slave_size_sb, SIGNAL(valueChanged(int)), creator, SLOT(change_unit_size()));
        size_box_layout->addWidget(master_label);
        size_box_layout->addWidget(master_size_sb);
        size_box_layout->addWidget(slave_label);
        size_box_layout->addWidget(slave_size_sb);
        size_box_layout->addStretch(1);
        sc_box_layout->addLayout(size_box_layout);
    }
    command_box = make_command_box();
    if (command_box != nullptr) sc_box_layout->addWidget(command_box);
    sc_box_layout->addStretch(1);
    sc_groupbox->setLayout(sc_box_layout);
    return sc_groupbox;
}

QGroupBox* OptionBox::make_command_box() {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        auto opt_command = p->handle->optional_command();
        std::unordered_map<CommandGroup, std::pair<std::string, double>> command;
        if (in_tree == InTree::ARMY) command = p->command();
        if (opt_command.empty()) return nullptr;
        QGroupBox* gb = new QGroupBox(creator->tr("Command Group"));
        QVBoxLayout* vbl = new QVBoxLayout;
        command_box_helper(vbl, opt_command, command);
        vbl->addStretch(1);
        gb->setLayout(vbl);
        return gb;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        auto opt_master_command = p->master().handle->optional_command();
        auto opt_slave_command = p->slave().handle->optional_command();
        if (opt_master_command.empty() && opt_slave_command.empty()) return nullptr;
        std::unordered_map<CommandGroup, std::pair<std::string, double>> master_command;
        std::unordered_map<CommandGroup, std::pair<std::string, double>> slave_command;
        if (in_tree == InTree::ARMY) {
            master_command = p->master().command();
            slave_command = p->slave().command();
        }
        QGroupBox* gb = new QGroupBox(creator->tr("Command Groups"));
        QHBoxLayout* hlayout = new QHBoxLayout;
        // master command group
        QGroupBox* master_gb = new QGroupBox();
        QVBoxLayout* master_vbl = new QVBoxLayout;
        command_box_helper(master_vbl, opt_master_command, master_command, true);
        master_vbl->addStretch(1);
        master_gb->setLayout(master_vbl);
        hlayout->addWidget(master_gb);
        // slave command group
        QGroupBox* slave_gb = new QGroupBox();
        QVBoxLayout* slave_vbl = new QVBoxLayout;
        command_box_helper(slave_vbl, opt_slave_command, slave_command);
        slave_vbl->addStretch(1);
        slave_gb->setLayout(slave_vbl);
        hlayout->addWidget(slave_gb);
        // now finalise both
        gb->setLayout(hlayout);
        return gb;
    }
    default: return nullptr;
    }
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_weapons_boxes() {
    bool has_opt_weapons = false;
    bool has_champ_opt_weapons = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().opt_weapons.empty()) return boxes;
        has_opt_weapons = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().opt_weapons.empty() && p->handle->champion_opt().opt_weapons.empty())
            return boxes;
        if (!p->handle->opt().opt_weapons.empty()) has_opt_weapons = true;
        if (!p->handle->champion_opt().opt_weapons.empty()) has_champ_opt_weapons = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_weapons) {
        QGroupBox* weapons_box = new QGroupBox(creator->tr("Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = make_weapons_subbox(WeaponType::MELEE, false);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = make_weapons_subbox(WeaponType::BALLISTIC, false);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.first = weapons_box;
    }
    if (has_champ_opt_weapons) {
        QGroupBox* weapons_box = new QGroupBox(creator->tr("Champion Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = make_weapons_subbox(WeaponType::MELEE, true);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = make_weapons_subbox(WeaponType::BALLISTIC, true);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.second = weapons_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_weapons_subbox(WeaponType wt, bool champion) {
    std::unordered_map<
        std::string,
        std::tuple<
            WeaponType,
            ItemClass,
            double,
            std::vector<std::string>
        >
    > opt_weapons;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > curr_weapons;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_weapons = p->handle_->opt().opt_weapons;
        curr_weapons = p->weapons();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_weapons = p->handle->champion_opt().opt_weapons;
            p->switch_model_select(ModelSelect::CHAMPION);
            curr_weapons = p->weapons();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_weapons = p->handle->opt().opt_weapons;
            p->switch_model_select(ModelSelect::DEFAULT);
            curr_weapons = p->weapons();
        }
        break;
    }
    default: return nullptr;
    }
    auto opt_weapon_vec = tools::find_all_if(
        std::begin(opt_weapons),
        std::end(opt_weapons),
        [wt] (const auto& x) { return std::get<0>(x.second) == wt; }
    );
    if (opt_weapon_vec.empty()) return nullptr;
    std::string subbox_label = "";
    switch (wt) {
    case WeaponType::MELEE:
        subbox_label = "Melee";
        break;
    case WeaponType::BALLISTIC:
        subbox_label = "Ranged";
        break;
    default: throw std::runtime_error("Unrecognised weapon type!");
    }
    QGroupBox* weapons_subbox = new QGroupBox(creator->tr(subbox_label.data()));
    QVBoxLayout* weapons_subbox_layout = new QVBoxLayout;
    bool has_weapon = false;
    for (const auto& w : opt_weapon_vec) {
        BaseUnitType but = current->base_unit_type();
        std::string pts_str = tools::points_str(std::get<2>(w->second), but);
        std::string button_label = w->first + " (" + pts_str + ")";
        std::string button_name = w->first + "_" +
                std::to_string(static_cast<int>(std::get<0>(w->second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(w->second))) + "_" +
                ((champion) ? "champion" : "default") + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr(button_label.data()));
        rb->setObjectName(QString(button_name.data()));
        // check if current unit weapon map contains this weapon
        if (curr_weapons.count(std::get<0>(w->second))) {
            if (std::get<1>(curr_weapons.at(std::get<0>(w->second))) == w->first) {
                rb->setChecked(true);
                has_weapon = true;
            }
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_weapon_selected()));
        weapons_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    std::string none_rb_name = "None_opt_";
    switch (wt) {
    case WeaponType::MELEE:
        none_rb_name += std::string("melee_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    case WeaponType::BALLISTIC:
        none_rb_name += std::string("ranged_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    default: throw std::runtime_error("Unrecognised weapon type!");
    }
    none_rb->setObjectName(QString(none_rb_name.data()));
    if (!has_weapon) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_weapon_selected()));
    weapons_subbox_layout->addWidget(none_rb);
    weapons_subbox_layout->addStretch(1);
    weapons_subbox->setLayout(weapons_subbox_layout);
    return weapons_subbox;
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_armour_boxes() {
    bool has_opt_armour = false;
    bool has_champ_opt_armour = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().opt_armour.empty()) return boxes;
        has_opt_armour = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().opt_armour.empty() && p->handle->champion_opt().opt_armour.empty())
            return boxes;
        if (!p->handle->opt().opt_armour.empty()) has_opt_armour = true;
        if (!p->handle->champion_opt().opt_armour.empty()) has_champ_opt_armour = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_armour) {
        QGroupBox* armour_box = new QGroupBox(creator->tr("Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = make_armour_subbox(ArmourType::ARMOUR, false);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = make_armour_subbox(ArmourType::SHIELD, false);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = make_armour_subbox(ArmourType::HELMET, false);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.first = armour_box;
    }
    if (has_champ_opt_armour) {
        QGroupBox* armour_box = new QGroupBox(creator->tr("Champion Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = make_armour_subbox(ArmourType::ARMOUR, true);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = make_armour_subbox(ArmourType::SHIELD, true);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = make_armour_subbox(ArmourType::HELMET, true);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.second = armour_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_armour_subbox(ArmourType at, bool champion) {
    std::unordered_map<
        std::string,
        std::tuple<
            ArmourType,
            ItemClass,
            double,
            std::vector<std::string>
        >
    > opt_armour;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > curr_armour;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_armour = p->handle_->opt().opt_armour;
        curr_armour = p->armour();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_armour = p->handle->champion_opt().opt_armour;
            p->switch_model_select(ModelSelect::CHAMPION);
            curr_armour = p->armour();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_armour = p->handle->opt().opt_armour;
            p->switch_model_select(ModelSelect::DEFAULT);
            curr_armour = p->armour();
        }
        break;
    }
    default: return nullptr;
    }
    auto opt_armour_vec = tools::find_all_if(
        std::begin(opt_armour),
        std::end(opt_armour),
        [at] (const auto& x) { return std::get<0>(x.second) == at; }
    );
    if (opt_armour_vec.empty()) return nullptr;
    std::string subbox_label = "";
    switch (at) {
    case ArmourType::ARMOUR:
        subbox_label = "Body Armour";
        break;
    case ArmourType::SHIELD:
        subbox_label = "Shield";
        break;
    case ArmourType::HELMET:
        subbox_label = "Helmet";
        break;
    default: throw std::runtime_error("Unrecognised armour type!");
    }
    QGroupBox* armour_subbox = new QGroupBox(creator->tr(subbox_label.data()));
    QVBoxLayout* armour_subbox_layout = new QVBoxLayout;
    bool has_armour = false;
    for (const auto& a : opt_armour_vec) {
        BaseUnitType but = current->base_unit_type();
        std::string pts_str = tools::points_str(std::get<2>(a->second), but);
        std::string button_label = a->first + " (" + pts_str + ")";
        std::string button_name = a->first + "_" +
                std::to_string(static_cast<int>(std::get<0>(a->second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(a->second))) + "_" +
                ((champion) ? "champion" : "default") + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr(button_label.data()));
        rb->setObjectName(QString(button_name.data()));
        // check if current unit armour map contains this piece of armour
        if (curr_armour.count(std::get<0>(a->second))) {
               if (std::get<1>(curr_armour.at(std::get<0>(a->second))) == a->first) {
                   rb->setChecked(true);
                   has_armour = true;
               }
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
        armour_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    std::string none_rb_name = "None_opt_";
    switch (at) {
    case ArmourType::ARMOUR:
        none_rb_name += std::string("armour_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    case ArmourType::SHIELD:
        none_rb_name += std::string("shield_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    case ArmourType::HELMET:
        none_rb_name += std::string("helmet_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    default: throw std::runtime_error("Unrecognised armour type!");
    }
    none_rb->setObjectName(QString(none_rb_name.data()));
    if (!has_armour) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
    armour_subbox_layout->addWidget(none_rb);
    armour_subbox_layout->addStretch(1);
    armour_subbox->setLayout(armour_subbox_layout);
    return armour_subbox;
}

QGroupBox* OptionBox::make_mage_levels_box() {
    auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
    std::unordered_map<short, double> opt_levels = p->handle->level_upgrades();
    short level = p->level();
    if (opt_levels.empty()) return nullptr;
    QGroupBox* levels_box = new QGroupBox(creator->tr("Mage Levels"));
    QVBoxLayout* levels_box_layout = new QVBoxLayout;
    bool has_level = false;
    std::string none_rb_name = "Level " + std::to_string(p->handle->mage_level()) + " (0 pts)";
    QRadioButton* none_rb = new QRadioButton(creator->tr(none_rb_name.data()));
    none_rb->setObjectName(QString("Default_levels_radiobutton"));
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_level_selected()));
    levels_box_layout->addWidget(none_rb);
    for (const auto& l : opt_levels) {
        auto tmp = tools::split(std::to_string(l.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = "Level " + std::to_string(l.first) + " (" + pts_str + " pts" + ")";
        QRadioButton* b = new QRadioButton(creator->tr(name.data()));
        b->setObjectName(QString(("level_" + std::to_string(l.first) + "_radiobutton").data()));
        if (level == l.first) {
            b->setChecked(true);
            has_level = true;
        }
        creator->connect(b, SIGNAL(clicked(bool)), creator, SLOT(optional_level_selected()));
        levels_box_layout->addWidget(b);
    }
    if (!has_level) none_rb->setChecked(true);
    levels_box_layout->addStretch(1);
    levels_box->setLayout(levels_box_layout);
    return levels_box;
}

QGroupBox* OptionBox::make_mounts_boxes() {
    std::unordered_map<std::string, std::pair<UnitClass, double>> opt_mounts;
    std::pair<std::string, std::pair<UnitClass, double>> mount;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_mounts = p->handle_->opt().opt_mounts;
        mount = p->mount();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_mounts = p->handle->opt().opt_mounts;
        mount = p->mount();
        break;
    }
    default: return nullptr;
    }
    if (opt_mounts.empty()) return nullptr;
    QGroupBox* mounts_box = new QGroupBox(creator->tr("Mounts"));
    QVBoxLayout* vbox_mounts = new QVBoxLayout;
    bool has_mount = false;
    for (const auto& m : opt_mounts) {
        auto tmp = tools::split(std::to_string(std::get<1>(m.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = m.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(creator->tr(name.data()));
        b->setObjectName(QString((m.first + "_radiobutton").data()));
        if (mount.first == m.first) {
            b->setChecked(true);
            has_mount = true;
        }
        creator->connect(b, SIGNAL(clicked(bool)), creator, SLOT(optional_mount_selected()));
        vbox_mounts->addWidget(b);
        // spawn another group box containing mount options
        if (current->base()->mounts_handle() == nullptr) continue;
        auto mount_meta = current->base()->mounts_handle()->find(m.first);
        if (mount_meta != current->base()->mounts_handle()->end() &&
                mount_meta->second.has_options()) {
            QGroupBox* mount_options_box = new QGroupBox(creator->tr("Options"));
            QVBoxLayout* mob_layout = new QVBoxLayout;
            for (const auto& mo : mount_meta->second.mc_extras) {
                std::string mo_name = mo.first + " (" + tools::points_str(mo.second) + " pts)";
                QCheckBox* cb = new QCheckBox(creator->tr(mo_name.data()));
                mob_layout->addWidget(cb);
                mob_layout->addStretch(1);
                mount_options_box->setLayout(mob_layout);
                vbox_mounts->addWidget(mount_options_box);
            }
        }
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    none_rb->setObjectName(QString("None_mounts_radiobutton"));
    if (!has_mount) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_mount_selected()));
    vbox_mounts->addWidget(none_rb);
    vbox_mounts->addStretch(1);
    mounts_box->setLayout(vbox_mounts);
    return mounts_box;
}

std::pair<QGroupBox*, QGroupBox*> OptionBox::make_extras_boxes() {
    bool has_opt_oco_extras = false;
    bool has_champ_opt_oco_extras = false;
    bool has_opt_mc_extras = false;
    bool has_champ_opt_mc_extras = false;
    std::pair<QGroupBox*, QGroupBox*> boxes = {nullptr, nullptr};
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        if (p->handle_->opt().oco_extras.empty() && p->handle_->opt().mc_extras.empty())
            return boxes;
        if (!p->handle_->opt().oco_extras.empty()) has_opt_oco_extras = true;
        if (!p->handle_->opt().mc_extras.empty()) has_opt_mc_extras = true;
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (p->handle->opt().oco_extras.empty() && p->handle->opt().mc_extras.empty() &&
                p->handle->champion_opt().oco_extras.empty() && p->handle->champion_opt().mc_extras.empty())
            return boxes;
        if (!p->handle->opt().oco_extras.empty()) has_opt_oco_extras = true;
        if (!p->handle->opt().mc_extras.empty()) has_opt_mc_extras = true;
        if (!p->handle->champion_opt().oco_extras.empty()) has_champ_opt_oco_extras = true;
        if (!p->handle->champion_opt().mc_extras.empty()) has_champ_opt_mc_extras = true;
        break;
    }
    default: return boxes;
    }
    if (has_opt_oco_extras || has_opt_mc_extras) {
        QGroupBox* extras_box = new QGroupBox(creator->tr("Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_opt_oco_extras) {
            auto oco_subbox = make_oco_extras_subbox(false);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_opt_mc_extras) {
            auto mc_subbox = make_mc_extras_subbox(false);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.first = extras_box;
    }
    if (has_champ_opt_oco_extras || has_champ_opt_mc_extras) {
        QGroupBox* extras_box = new QGroupBox(creator->tr("Champion Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_champ_opt_oco_extras) {
            auto oco_subbox = make_oco_extras_subbox(true);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_champ_opt_mc_extras) {
            auto mc_subbox = make_mc_extras_subbox(true);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.second = extras_box;
    }
    return boxes;
}

QGroupBox* OptionBox::make_oco_extras_subbox(bool champion) {
    std::unordered_map<std::string, std::pair<bool, double>> opt_oco_extras;
    std::pair<std::string, std::pair<bool, double>> curr_oco_extra;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_oco_extras = p->handle_->opt().oco_extras;
        if (in_tree == InTree::ARMY) curr_oco_extra = p->oco_extra();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_oco_extras = p->handle->champion_opt().oco_extras;
            p->switch_model_select(ModelSelect::CHAMPION);
            if (in_tree == InTree::ARMY) curr_oco_extra = p->oco_extra();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_oco_extras = p->handle->opt().oco_extras;
            p->switch_model_select(ModelSelect::DEFAULT);
            if (in_tree == InTree::ARMY) curr_oco_extra = p->oco_extra();
        }
        break;
    }
    default: return nullptr;
    }
    QGroupBox* oco_box = new QGroupBox();
    QVBoxLayout* oco_box_layout = new QVBoxLayout;
    bool has_extra = false;
    for (const auto& e : opt_oco_extras) {
        std::string pts_str = tools::points_str(e.second.second);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL &&
                                    !(e.second.first)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        std::string name = e.first + "_" + ((champion) ? "champion" : "default") + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr(label.data()));
        rb->setObjectName(QString(name.data()));
        if (curr_oco_extra.first == e.first) {
            rb->setChecked(true);
            has_extra = true;
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_oco_extra_selected()));
        oco_box_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    std::string none_rb_name = std::string("None_") + ((champion) ? "champion" : "default") + "_ocoextra_radiobutton";
    none_rb->setObjectName(QString(none_rb_name.data()));
    if (!has_extra) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_oco_extra_selected()));
    oco_box_layout->addWidget(none_rb);
    oco_box_layout->addStretch(1);
    oco_box->setLayout(oco_box_layout);
    return oco_box;
}

QGroupBox* OptionBox::make_mc_extras_subbox(bool champion) {
    std::unordered_map<std::string, std::pair<bool, double>> opt_mc_extras;
    std::unordered_map<std::string, std::pair<bool, double>> curr_mc_extras;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_mc_extras = p->handle_->opt().mc_extras;
        if (in_tree == InTree::ARMY) curr_mc_extras = p->mc_extras();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_mc_extras = p->handle->champion_opt().mc_extras;
            p->switch_model_select(ModelSelect::CHAMPION);
            if (in_tree == InTree::ARMY) curr_mc_extras = p->mc_extras();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_mc_extras = p->handle->opt().mc_extras;
            p->switch_model_select(ModelSelect::DEFAULT);
            if (in_tree == InTree::ARMY) curr_mc_extras = p->mc_extras();
        }
        break;
    }
    default: return nullptr;
    }
    QGroupBox* mc_box = new QGroupBox();
    QVBoxLayout* mc_box_layout = new QVBoxLayout;
    for (const auto& e : opt_mc_extras) {
        std::string pts_str = tools::points_str(e.second.second);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL &&
                                    !(e.second.first)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        std::string name = e.first + "_" + ((champion) ? "champion" : "default") + "_radiobutton";
        QCheckBox* cb = new QCheckBox(creator->tr(label.data()));
        cb->setObjectName(QString(name.data()));
        if (curr_mc_extras.count(e.first)) cb->setChecked(true);
        creator->connect(cb, SIGNAL(clicked(bool)), creator, SLOT(optional_mc_extra_selected()));
        mc_box_layout->addWidget(cb);
    }
    mc_box_layout->addStretch(1);
    mc_box->setLayout(mc_box_layout);
    return mc_box;
}
