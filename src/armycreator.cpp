#include "armycreator.h"
#include "ui_armycreator.h"

ArmyCreator::ArmyCreator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmyCreator) {
    ui->setupUi(this);
    race = armies::s_map_string_faction[
        ui->faction_combobox->currentText().toStdString()
    ];
    army = std::make_shared<army_list>(ui->pts_limit_spinbox->value());
    st = std::make_shared<selection_tree>(race, *army);
    id_counter = 0;
    in_tree = InTree::NEITHER;
    opt_sel = std::make_shared<option_selector>(st, army);

    ui->army_tree->header()->resizeSection(0, 250); // unit name header
    ui->army_tree->header()->resizeSection(1, 60); // unit size header
    ui->army_tree->header()->resizeSection(2, 180); // weapons header
    ui->army_tree->header()->resizeSection(3, 150); // armour header
    ui->army_tree->header()->resizeSection(4, 180); // command group header
    ui->army_tree->header()->resizeSection(5, 180); // extras header
    ui->army_tree->header()->resizeSection(6, 60); // points header
    ui->remove_button->setEnabled(false);
    populate_roster_tree();
}

ArmyCreator::~ArmyCreator() {
    delete ui;
}

void ArmyCreator::populate_roster_tree() {
    auto lords = st->lords();
    auto heroes = st->heroes();
    auto cores = st->core();
    auto specials = st->special();
    auto rares = st->rare();
    for (const auto& l : lords) {
        QTreeWidgetItem* lord_item = new QTreeWidgetItem();
        lord_item->setText(0, QString(l->name().data()));
        ui->roster_tree->topLevelItem(0)->addChild(lord_item);
        ui->roster_tree->topLevelItem(0)->setExpanded(true);
    }
    for (const auto& h : heroes) {
        QTreeWidgetItem* hero_item = new QTreeWidgetItem();
        hero_item->setText(0, QString(h->name().data()));
        ui->roster_tree->topLevelItem(1)->addChild(hero_item);
        ui->roster_tree->topLevelItem(1)->setExpanded(true);
    }
    for (const auto& c : cores) {
        QTreeWidgetItem* core_item = new QTreeWidgetItem();
        core_item->setText(0, QString(c->name().data()));
        ui->roster_tree->topLevelItem(2)->addChild(core_item);
        ui->roster_tree->topLevelItem(2)->setExpanded(true);
    }
    for (const auto& s : specials) {
        QTreeWidgetItem* spec_item = new QTreeWidgetItem();
        spec_item->setText(0, QString(s->name().data()));
        ui->roster_tree->topLevelItem(3)->addChild(spec_item);
        ui->roster_tree->topLevelItem(3)->setExpanded(true);
    }
    for (const auto& r : rares) {
        QTreeWidgetItem* rare_item = new QTreeWidgetItem();
        rare_item->setText(0, QString(r->name().data()));
        ui->roster_tree->topLevelItem(4)->addChild(rare_item);
        ui->roster_tree->topLevelItem(4)->setExpanded(true);
    }
}

void ArmyCreator::clear_roster_tree() {
    for (int i = 0; i < 5; ++i)
        ui->roster_tree->topLevelItem(i)->takeChildren();
}

void ArmyCreator::clear_army_tree() {
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->takeChildren();
}

void ArmyCreator::clear_unit_options_box() {
    auto c = ui->options_group_box->children();
    for (auto& x : c) delete x;
}

void ArmyCreator::optional_weapon_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    if (opt_sel->select_weapon(rb_name)) {
        ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
        update_unit_display(ui->army_tree->currentItem());
    }
}

void ArmyCreator::optional_armour_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    if (opt_sel->select_armour(rb_name)) {
        ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
        update_unit_display(ui->army_tree->currentItem());
    }
}

void ArmyCreator::optional_command_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    if (opt_sel->select_command(cb_name, cb->isChecked())) {
        ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
        update_unit_display(ui->army_tree->currentItem());
    }
}

void ArmyCreator::optional_oco_extra_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    if (opt_sel->select_oco_extra(rb_name)) {
        ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
        update_unit_display(ui->army_tree->currentItem());
    }
}

void ArmyCreator::optional_mc_extra_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    if (opt_sel->select_mc_extra(cb_name, cb->isChecked())) {
        ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
        update_unit_display(ui->army_tree->currentItem());
    }
}

QGroupBox* ArmyCreator::init_opt_subweapons_groupbox(
    WeaponType wt,
    const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& opt_weapons,
    const std::unordered_map<WeaponType, std::tuple<ItemClass, std::string, double>>& weapons,
    int id
) {
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
    default: break;
    }
    QGroupBox* weapons_subbox = new QGroupBox(tr(subbox_label.data()));
    QVBoxLayout* weapons_subbox_layout = new QVBoxLayout;
    bool has_weapon = false;
    for (const auto& w : opt_weapon_vec) {
        BaseUnitType but;
        if (in_tree == InTree::ROSTER) but = st->selected()->base_unit_type();
        else but = army->get_unit(id)->base_unit_type();
        std::string pts_str = tools::points_str(std::get<2>(w->second), but);
        std::string button_label = w->first + " (" + pts_str + ")";
        std::string button_name = w->first + "_" +
                std::to_string(static_cast<int>(std::get<0>(w->second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(w->second))) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(tr(button_label.data()));
        rb->setObjectName(tr(button_name.data()));
        // check if current unit weapon map contains this weapon
        if (weapons.count(std::get<0>(w->second))) {
            if (std::get<1>(weapons.at(std::get<0>(w->second))) == w->first) {
                rb->setChecked(true);
                has_weapon = true;
            }
        }
        connect(rb, SIGNAL(clicked(bool)), this, SLOT(optional_weapon_selected()));
        weapons_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    std::string none_rb_name = "None_opt_";
    switch (wt) {
    case WeaponType::MELEE:
        none_rb_name += "melee_radiobutton";
        break;
    case WeaponType::BALLISTIC:
        none_rb_name += "ranged_radiobutton";
        break;
    default: throw std::runtime_error("Unrecognised weapon type!");
    }
    none_rb->setObjectName(tr(none_rb_name.data()));
    if (!has_weapon) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_weapon_selected()));
    weapons_subbox_layout->addWidget(none_rb);
    weapons_subbox_layout->addStretch(1);
    weapons_subbox->setLayout(weapons_subbox_layout);
    return weapons_subbox;
}

QGroupBox* ArmyCreator::init_opt_weapons_groupbox(
        const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& opt_weapons,
        const std::unordered_map<WeaponType, std::tuple<ItemClass, std::string, double>>& weapons,
        int id
     ) {
    QGroupBox* weapons_box = new QGroupBox(tr("Weapons"));
    QVBoxLayout* vbox_weapons = new QVBoxLayout;
    // create the sub group boxes for melee and ranged weapon options
    auto melee_subbox = init_opt_subweapons_groupbox(WeaponType::MELEE, opt_weapons, weapons, id);
    if (melee_subbox != nullptr) vbox_weapons->addWidget(melee_subbox);
    auto ranged_subbox = init_opt_subweapons_groupbox(WeaponType::BALLISTIC, opt_weapons, weapons, id);
    if (ranged_subbox != nullptr) vbox_weapons->addWidget(ranged_subbox);
    vbox_weapons->addStretch(1);
    weapons_box->setLayout(vbox_weapons);
    return weapons_box;
}

QGroupBox* ArmyCreator::init_opt_subarmour_groupbox(
    ArmourType at,
    const std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>>& opt_armour,
    const std::unordered_map<ArmourType, std::tuple<ItemClass, std::string, double>>& armour,
    int id
) {
    auto opt_armour_vec = tools::find_all_if(
        std::begin(opt_armour),
        std::end(opt_armour),
        [at](const auto& x) { return std::get<0>(x.second) == at; }
    );
    if (opt_armour_vec.empty()) return nullptr;
    QGroupBox* ab_armours_box = new QGroupBox();
    QVBoxLayout* ab_armours_box_layout = new QVBoxLayout;
    bool has_armour = false;
    for (const auto& a : opt_armour_vec) {
        BaseUnitType but;
        if (in_tree == InTree::ROSTER) but = st->selected()->base_unit_type();
        else but = army->get_unit(id)->base_unit_type();
        std::string pts_str = tools::points_str(std::get<2>(a->second), but);
        std::string button_label = a->first + " (" + pts_str + ")";
        std::string button_name = a->first + "_" +
                std::to_string(static_cast<int>(std::get<0>(a->second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(a->second))) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(tr(button_label.data()));
        rb->setObjectName(tr(button_name.data()));
        // check if current unit armour map contains this piece of armour
        if (armour.count(std::get<0>(a->second))) {
               if (std::get<1>(armour.at(std::get<0>(a->second))) == a->first) {
                   rb->setChecked(true);
                   has_armour = true;
               }
        }
        connect(rb, SIGNAL(clicked(bool)), this, SLOT(optional_armour_selected()));
        ab_armours_box_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    std::string none_rb_name = "None_opt_";
    switch (at) {
    case ArmourType::ARMOUR:
        none_rb_name += "armour_radiobutton";
        break;
    case ArmourType::SHIELD:
        none_rb_name += "shield_radiobutton";
        break;
    case ArmourType::HELMET:
        none_rb_name += "helmet_radiobutton";
        break;
    default: throw std::runtime_error("Unrecognised armour type!");
    }
    none_rb->setObjectName(tr(none_rb_name.data()));
    if (!has_armour) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_armour_selected()));
    ab_armours_box_layout->addWidget(none_rb);
    ab_armours_box_layout->addStretch(1);
    ab_armours_box->setLayout(ab_armours_box_layout);
    return ab_armours_box;
}

QGroupBox* ArmyCreator::init_opt_armour_groupbox(
        const std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>>& opt_armour,
        const std::unordered_map<ArmourType, std::tuple<ItemClass, std::string, double>>& armour,
        int id
     ) {
    QGroupBox* armours_box = new QGroupBox(tr("Armours"));
    QVBoxLayout* vbox_armours = new QVBoxLayout;
    // create the sub group boxes for armour, shield and helmet options
    auto armours_subbox = init_opt_subarmour_groupbox(ArmourType::ARMOUR, opt_armour, armour, id);
    if (armours_subbox != nullptr) vbox_armours->addWidget(armours_subbox);
    auto shields_subbox = init_opt_subarmour_groupbox(ArmourType::SHIELD, opt_armour, armour, id);
    if (shields_subbox != nullptr) vbox_armours->addWidget(shields_subbox);
    auto helmets_subbox = init_opt_subarmour_groupbox(ArmourType::HELMET, opt_armour, armour, id);
    if (helmets_subbox != nullptr) vbox_armours->addWidget(helmets_subbox);
    vbox_armours->addStretch(1);
    armours_box->setLayout(vbox_armours);
    return armours_box;
}

QGroupBox* ArmyCreator::init_opt_mounts_groupbox(
        const std::unordered_map<std::string, std::pair<armies::UnitClass, double>>& mounts,
        int id
     ) {
    QGroupBox* mounts_box = new QGroupBox(tr("Mounts"));
    QVBoxLayout* vbox_mounts = new QVBoxLayout;
    for (const auto& m : mounts) {
        auto tmp = tools::split(std::to_string(std::get<1>(m.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = "";
        if (in_tree == InTree::ROSTER)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = m.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(tr(name.data()));
        b->setObjectName(tr((m.first + "_radiobutton").data()));
        vbox_mounts->addWidget(b);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    none_rb->setObjectName(tr("None_radiobutton"));
    none_rb->setChecked(true);
    vbox_mounts->addWidget(none_rb);
    vbox_mounts->addStretch(1);
    mounts_box->setLayout(vbox_mounts);
    return mounts_box;
}

QGroupBox* ArmyCreator::init_opt_oco_extras_groupbox(
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& opt_oco_extras,
    const std::pair<std::string, std::pair<bool, double>>& curr_oco_extra,
    int id
) {
    if (opt_oco_extras.empty()) return nullptr;
    QGroupBox* oco_box = new QGroupBox();
    QVBoxLayout* oco_box_layout = new QVBoxLayout;
    bool has_extra = false;
    for (const auto& e : opt_oco_extras) {
        std::string pts_str = tools::points_str(e.second.second);
        std::string permodel = "";
        if (in_tree == InTree::ROSTER)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL &&
                !(e.second.first)) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL &&
                !(e.second.first)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        std::string name = e.first + "_radiobutton";
        QRadioButton* rb = new QRadioButton(tr(label.data()));
        rb->setObjectName(tr(name.data()));
        if (curr_oco_extra.first == e.first) {
            rb->setChecked(true);
            has_extra = true;
        }
        connect(rb, SIGNAL(clicked(bool)), this, SLOT(optional_oco_extra_selected()));
        oco_box_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    none_rb->setObjectName(tr("None_ocoextra_radiobutton"));
    if (!has_extra) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_oco_extra_selected()));
    oco_box_layout->addWidget(none_rb);
    oco_box_layout->addStretch(1);
    oco_box->setLayout(oco_box_layout);
    return oco_box;
}

QGroupBox* ArmyCreator::init_opt_mc_extras_groupbox(
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& opt_mc_extras,
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    > curr_mc_extras,
    int id
) {
    if (opt_mc_extras.empty()) return nullptr;
    QGroupBox* mc_box = new QGroupBox();
    QVBoxLayout* mc_box_layout = new QVBoxLayout;
    for (const auto& e : opt_mc_extras) {
        std::string pts_str = tools::points_str(e.second.second);
        std::string permodel = "";
        if (in_tree == InTree::ROSTER)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL &&
                !(e.second.first)) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL &&
                !(e.second.first)) ? "/model" : "";
        std::string label = e.first + " (" + pts_str + " pts" + permodel + ")";
        std::string name = e.first + "_checkbox";
        QCheckBox* cb = new QCheckBox(tr(label.data()));
        cb->setObjectName(tr(name.data()));
        if (curr_mc_extras.count(e.first)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_mc_extra_selected()));
        mc_box_layout->addWidget(cb);
    }
    mc_box_layout->addStretch(1);
    mc_box->setLayout(mc_box_layout);
    return mc_box;
}

QGroupBox* ArmyCreator::init_opt_extras_groupbox(
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& opt_oco_extras,
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    >& opt_mc_extras,
    const std::pair<std::string, std::pair<bool, double>>& curr_oco_extra,
    const std::unordered_map<
        std::string,
        std::pair<bool, double>
    > curr_mc_extras,
    int id
) {
    QGroupBox* extras_box = new QGroupBox(tr("Other"));
    QVBoxLayout* vbox_extras = new QVBoxLayout;
    auto oco_extras_subbox = init_opt_oco_extras_groupbox(opt_oco_extras, curr_oco_extra, id);
    if (oco_extras_subbox != nullptr) vbox_extras->addWidget(oco_extras_subbox);
    auto mc_extras_subbox = init_opt_mc_extras_groupbox(opt_mc_extras, curr_mc_extras, id);
    if (mc_extras_subbox != nullptr) vbox_extras->addWidget(mc_extras_subbox);
    vbox_extras->addStretch(1);
    extras_box->setLayout(vbox_extras);
    return extras_box;
}

QGroupBox* ArmyCreator::init_command_groupbox(
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& opt_command,
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& command
    ) {
    if (opt_command.empty()) return nullptr;
    QGroupBox* gb = new QGroupBox(tr("Command Group"));
    QVBoxLayout* vb = new QVBoxLayout;
    if (opt_command.count(CommandGroup::MUSICIAN)) {
        auto musician = opt_command.at(CommandGroup::MUSICIAN);
        auto tmp = tools::split(std::to_string(musician.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = musician.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(tr(name.data()));
        cb->setObjectName(tr((musician.first + "_m_checkbox").data()));
        if (command.count(CommandGroup::MUSICIAN)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_command_selected()));
        vb->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::STANDARD_BEARER)) {
        auto sb = opt_command.at(CommandGroup::STANDARD_BEARER);
        auto tmp = tools::split(std::to_string(sb.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = sb.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(tr(name.data()));
        cb->setObjectName(tr((sb.first + "_sb_checkbox").data()));
        if (command.count(CommandGroup::STANDARD_BEARER)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_command_selected()));
        vb->addWidget(cb);
    }
    if (opt_command.count(CommandGroup::CHAMPION)) {
        auto champ = opt_command.at(CommandGroup::CHAMPION);
        auto tmp = tools::split(std::to_string(champ.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string name = champ.first + " (" + pts_str + " pts)";
        QCheckBox* cb = new QCheckBox(tr(name.data()));
        cb->setObjectName(tr((champ.first + "_c_checkbox").data()));
        if (command.count(CommandGroup::CHAMPION)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_command_selected()));
        vb->addWidget(cb);
    }
    vb->addStretch(1);
    gb->setLayout(vb);
    return gb;
}

void ArmyCreator::change_unit_size() {
    QSpinBox* sb = qobject_cast<QSpinBox*>(QObject::sender());
    std::shared_ptr<unit> current;
    int curr_id = -1;
    switch (in_tree) {
    case InTree::ARMY:
        curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
        break;
    case InTree::ROSTER:
        current = st->selected();
        break;
    default: throw std::runtime_error("No unit selected!");
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        try {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(curr_id);
                p->change_size(static_cast<std::size_t>(sb->value()));
                army->update_on(curr_id);
                ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
                update_unit_display(ui->army_tree->currentItem());
                break;
            case InTree::ROSTER:
                p->change_size(static_cast<std::size_t>(sb->value()));
                break;
            default: throw std::runtime_error("No unit selected!");
            }
        } catch (const std::invalid_argument&) {}
        break;
    }
    default: break;
    }
}

QGroupBox* ArmyCreator::init_size_command_groupbox() {
    QGroupBox* sc_groupbox = new QGroupBox(tr("Size and Command"));
    QVBoxLayout* vbox = new QVBoxLayout;
    QHBoxLayout* hbox = new QHBoxLayout;
    // unit size
    QLabel* label = new QLabel(tr("Unit size"));
    QSpinBox* size_sb = new QSpinBox();
    // command group
    QGroupBox* command_buttons;
    std::unordered_map<
        CommandGroup, std::pair<std::string, double>
    > opt_command;
    std::shared_ptr<unit> current;
    if (in_tree == InTree::ROSTER) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_command = p->handle->optional_command();
        command_buttons = init_command_groupbox(opt_command, p->command());
        size_sb->setMinimum(static_cast<int>(p->min_size()));
        if (p->max_size() == std::numeric_limits<std::size_t>::max())
            size_sb->setMaximum(1000);
        else size_sb->setMaximum(static_cast<int>(p->max_size()));
        std::string size_sb_name = p->name() + "_spinbox";
        size_sb->setObjectName(QString(size_sb_name.data()));
        // set the value of the spinbox to the current unit size if army tree unit is selected
        if (in_tree == InTree::ARMY) size_sb->setValue(static_cast<int>(p->size()));
        connect(size_sb, SIGNAL(valueChanged(int)), this, SLOT(change_unit_size()));
        hbox->addWidget(label);
        hbox->addWidget(size_sb);
        hbox->addStretch(1);
        vbox->addLayout(hbox);
        break;
    }
    default:
        delete size_sb;
        delete label;
        delete hbox;
        delete vbox;
        delete sc_groupbox;
        return nullptr;
    }
    if (command_buttons != nullptr) vbox->addWidget(command_buttons);
    vbox->addStretch(1);
    sc_groupbox->setLayout(vbox);
    return sc_groupbox;
}

void ArmyCreator::initialise_unit_options_box() {
    // optional and current weapons
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > opt_weapons;
    std::unordered_map<
        WeaponType,
        std::tuple<ItemClass, std::string, double>
    > current_weapons;
    // optional and current armours
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > opt_armours;
    std::unordered_map<
        ArmourType,
        std::tuple<ItemClass, std::string, double>
    > current_armours;
    // optional and current mounts
    std::unordered_map<std::string, std::pair<armies::UnitClass, double>> opt_mounts;
    // optional and current extras
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > opt_oco_extras;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > opt_mc_extras;
    std::pair<std::string, std::pair<bool, double>> curr_oco_extra;
    std::unordered_map<
        std::string,
        std::pair<bool, double>
    > curr_mc_extras;
    int curr_id = 0;
    // overall layout for options box
    QVBoxLayout* vbox = new QVBoxLayout;
    auto sc_box = init_size_command_groupbox();
    if (sc_box != nullptr) vbox->addWidget(sc_box);
    std::shared_ptr<unit> current;
    if (in_tree == InTree::ROSTER) current = st->selected();
    else {
        curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
        opt_weapons = p->handle->opt().opt_weapons;
        opt_armours = p->handle->opt().opt_armour;
        opt_mounts = p->handle->opt().opt_mounts;
        opt_oco_extras = p->handle->opt().oco_extras;
        opt_mc_extras = p->handle->opt().mc_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
        curr_oco_extra = p->oco_extra();
        curr_mc_extras = p->mc_extras();
        //auto tmp_oco = p->oco_extra();
        /*curr_oco_extra = {tmp_oco.first, {true, tmp_oco.second}};
        auto tmp_mc = p->mc_extras();
        for (auto x : tmp_mc)
            curr_mc_extras[x.first] = {true, x.second};*/
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
        opt_weapons = p->handle->opt().opt_weapons;
        opt_armours = p->handle->opt().opt_armour;
        opt_mounts = p->handle->opt().opt_mounts;
        opt_oco_extras = p->handle->opt().oco_extras;
        opt_mc_extras = p->handle->opt().mc_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
        curr_oco_extra = p->oco_extra();
        curr_mc_extras = p->mc_extras();
        /*auto tmp_oco = p->oco_extra();
        curr_oco_extra = {tmp_oco.first, {true, tmp_oco.second}};
        auto tmp_mc = p->mc_extras();
        for (auto x : tmp_mc)
            curr_mc_extras[x.first] = {true, x.second};*/
        break;
    }
    case BaseUnitType::MIXED:
        // TODO: implement once mixed units are figured out
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_weapons = p->handle->opt().opt_weapons;
        opt_armours = p->handle->opt().opt_armour;
        opt_mounts = p->handle->opt().opt_mounts;
        opt_oco_extras = p->handle->opt().oco_extras;
        opt_mc_extras = p->handle->opt().mc_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
        curr_oco_extra = p->oco_extra();
        curr_mc_extras = p->mc_extras();
        break;
    }
    default:
        break;
    }
    if (!opt_weapons.empty())
        vbox->addWidget(init_opt_weapons_groupbox(opt_weapons, current_weapons, curr_id));
    if (!opt_armours.empty())
        vbox->addWidget(init_opt_armour_groupbox(opt_armours, current_armours, curr_id));
    if (!opt_mounts.empty())
        vbox->addWidget(init_opt_mounts_groupbox(opt_mounts, curr_id));
    if (!(opt_oco_extras.empty() && opt_mc_extras.empty()))
        vbox->addWidget(init_opt_extras_groupbox(
            opt_oco_extras,
            opt_mc_extras,
            curr_oco_extra,
            curr_mc_extras,
            curr_id
        ));
    // set overall layout
    ui->options_group_box->setLayout(vbox);
}

void ArmyCreator::on_actionExit_triggered() {
    QCoreApplication::quit();
}

void ArmyCreator::on_faction_combobox_currentTextChanged(const QString& faction) {
    race = armies::s_map_string_faction[
        faction.toStdString()
    ];
    army->clear();
    ui->current_pts_label->setText(QString("%1").arg(static_cast<double>(0.0)));
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->setText(5, QString("%1").arg(static_cast<double>(0.0)));
    st->reset(race, *army);
    id_counter = 0;
    // now fill the roster tree widget
    clear_roster_tree();
    clear_army_tree();
    clear_unit_options_box();
    populate_roster_tree();
}

void ArmyCreator::on_pts_limit_spinbox_valueChanged(double pts) {
    army->change_points_limit(pts);
}

void ArmyCreator::on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        st->change_selection(name);
        in_tree = InTree::ROSTER;
        ui->add_button->setEnabled(true);
        clear_unit_options_box();
        initialise_unit_options_box();
        opt_sel->reset(st->selected(), in_tree);
    }
    else {
        ui->add_button->setEnabled(false);
        clear_unit_options_box();
    }
    ui->army_tree->clearSelection();
}

void ArmyCreator::on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    if (item == nullptr) return;
    std::string name = item->text(column).toStdString();
    if (name == "Lords" || name == "Heroes" || name == "Core" ||
            name == "Special" || name == "Rare") return;
    if (st->selected()->name() != name) st->change_selection(name);
    on_add_button_clicked();
}

void ArmyCreator::on_army_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        in_tree = InTree::ARMY;
        ui->remove_button->setEnabled(true);
        clear_unit_options_box();
        initialise_unit_options_box();
        opt_sel->reset(army->get_unit(current->data(0, Qt::UserRole).toInt()), in_tree);
    }
    else {
        ui->remove_button->setEnabled(false);
        clear_unit_options_box();
    }
    ui->roster_tree->clearSelection();
}

void ArmyCreator::on_add_button_clicked() {
    st->add_unit_to_army_list(id_counter++);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    std::shared_ptr<unit> u = st->selected();
    // assign id
    QVariant v(u->id());
    item->setData(0, Qt::UserRole, v);
    // name column
    item->setText(0, QString(u->name().data()));
    // weapons column
    auto weapons = u->weapons();
    QString weapons_str("");
    auto melee_weapon = weapons[WeaponType::MELEE];
    auto ranged_weapon = weapons[WeaponType::BALLISTIC];
    if (!(std::get<1>(melee_weapon).empty()))
        weapons_str += QString("%1 [%2]").arg(
                           std::get<1>(melee_weapon).data(),
                           QString("%1").arg(std::get<2>(melee_weapon))
                       );
    if (!(std::get<1>(ranged_weapon).empty()))
        weapons_str += QString("\n") + QString("%1 [%2]").arg(
                           std::get<1>(ranged_weapon).data(),
                           QString("%1").arg(std::get<2>(ranged_weapon))
                       );
    item->setText(2, weapons_str);
    // armour column
    QString armour_str("");
    auto all_armour = u->armour();
    auto armour = all_armour[ArmourType::ARMOUR];
    auto shield = all_armour[ArmourType::SHIELD];
    auto helmet = all_armour[ArmourType::HELMET];
    if (!(std::get<1>(armour).empty()))
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(armour).data(),
                        QString("%1").arg(std::get<2>(armour))
                    );
    if (!(std::get<1>(shield).empty())) {
        if (!armour_str.isEmpty()) armour_str += QString("\n");
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(shield).data(),
                        QString("%1").arg(std::get<2>(shield))
                    );
    }
    if (!(std::get<1>(helmet).empty()))
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(helmet).data(),
                        QString("%1").arg(std::get<2>(helmet))
                    );
    item->setText(3, armour_str);
    // points column
    item->setText(6, QString("%1").arg(u->points()));
    switch (st->selected()->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        // size column
        item->setText(1, QString("%1").arg(1));
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(st->selected());
        // size column
        item->setText(1, QString("%1").arg(p->size()));
        // current command
        auto command = p->command();
        auto musician_it = command.find(CommandGroup::MUSICIAN);
        auto sb_it = command.find(CommandGroup::STANDARD_BEARER);
        auto champion_it = command.find(CommandGroup::CHAMPION);
        QString command_str("");
        if (musician_it != command.end()) {
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(musician_it->second).data(),
                              QString("%1").arg(std::get<1>(musician_it->second))
                          );
        }
        if (sb_it != command.end()) {
            if (musician_it != command.end()) command_str += QString("\n");
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(sb_it->second).data(),
                              QString("%1").arg(std::get<1>(sb_it->second))
                          );
        }
        if (champion_it != command.end()) {
            if (sb_it != command.end() || musician_it != command.end()) command_str += QString("\n");
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(champion_it->second).data(),
                              QString("%1").arg(std::get<1>(champion_it->second))
                          );
        }
        item->setText(4, command_str);
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(st->selected());
        // size column
        item->setText(1, QString("%1").arg(p->slave().size()));
        break;
    }
    default: break;
    }
    // unit type points column
    switch (st->selected()->unit_type()) {
    case armies::UnitType::LORD:
        ui->army_tree->topLevelItem(0)->addChild(item);
        ui->army_tree->topLevelItem(0)->setText(6, QString("%1").arg(army->lord_points()));
        ui->army_tree->topLevelItem(0)->setExpanded(true);
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->addChild(item);
        ui->army_tree->topLevelItem(1)->setText(6, QString("%1").arg(army->hero_points()));
        ui->army_tree->topLevelItem(1)->setExpanded(true);
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->addChild(item);
        ui->army_tree->topLevelItem(2)->setText(6, QString("%1").arg(army->core_points()));
        ui->army_tree->topLevelItem(2)->setExpanded(true);
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->addChild(item);
        ui->army_tree->topLevelItem(3)->setText(6, QString("%1").arg(army->special_points()));
        ui->army_tree->topLevelItem(3)->setExpanded(true);
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->addChild(item);
        ui->army_tree->topLevelItem(4)->setText(6, QString("%1").arg(army->rare_points()));
        ui->army_tree->topLevelItem(4)->setExpanded(true);
        break;
    default:
        delete item;
        break;
    }
    st->change_selection(st->selected()->name());
    clear_unit_options_box();
    // just make sure to keep us in roster tree
    // in case it gets changed somehow
    in_tree = InTree::ROSTER;
    initialise_unit_options_box();
}

void ArmyCreator::on_remove_button_clicked() {
    QTreeWidgetItem* item = ui->army_tree->currentItem();
    QVariant v = item->data(0, Qt::UserRole);
    int id = v.toInt();
    armies::UnitType unit_type = army->get_unit(id)->unit_type();
    army->remove_unit(id);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    switch (unit_type) {
    case armies::UnitType::LORD:
        ui->army_tree->topLevelItem(0)->setText(6, QString("%1").arg(army->lord_points()));
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->setText(6, QString("%1").arg(army->hero_points()));
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->setText(6, QString("%1").arg(army->core_points()));
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->setText(6, QString("%1").arg(army->special_points()));
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->setText(6, QString("%1").arg(army->rare_points()));
        break;
    default:
        break;
    }
    clear_unit_options_box();
    delete item;
}

void ArmyCreator::on_clear_button_clicked() {
    army->clear();
    ui->current_pts_label->setText(QString("%1").arg(static_cast<double>(0.0)));
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->setText(6, QString("%1").arg(static_cast<double>(0.0)));
    clear_army_tree();
    clear_unit_options_box();
}

void ArmyCreator::update_unit_display(QTreeWidgetItem* item) {
    int id = item->data(0, Qt::UserRole).toInt();
    std::shared_ptr<unit> u = army->get_unit(id);
    // name column
    item->setText(0, QString(u->name().data()));
    // weapons column
    auto weapons = u->weapons();
    QString weapons_str("");
    auto melee_weapon = weapons[WeaponType::MELEE];
    auto ranged_weapon = weapons[WeaponType::BALLISTIC];
    if (!(std::get<1>(melee_weapon).empty()))
        weapons_str += QString("%1 [%2]").arg(
                           std::get<1>(melee_weapon).data(),
                           QString("%1").arg(std::get<2>(melee_weapon))
                       );
    if (!(std::get<1>(ranged_weapon).empty()))
        weapons_str += QString("\n") + QString("%1 [%2]").arg(
                           std::get<1>(ranged_weapon).data(),
                           QString("%1").arg(std::get<2>(ranged_weapon))
                       );
    item->setText(2, weapons_str);
    // armour column
    QString armour_str("");
    auto all_armour = u->armour();
    auto armour = all_armour[ArmourType::ARMOUR];
    auto shield = all_armour[ArmourType::SHIELD];
    auto helmet = all_armour[ArmourType::HELMET];
    if (!(std::get<1>(armour).empty()))
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(armour).data(),
                        QString("%1").arg(std::get<2>(armour))
                    );
    if (!(std::get<1>(shield).empty())) {
        if (!armour_str.isEmpty()) armour_str += QString("\n");
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(shield).data(),
                        QString("%1").arg(std::get<2>(shield))
                    );
    }
    if (!(std::get<1>(helmet).empty()))
        armour_str += QString("%1 [%2]").arg(
                        std::get<1>(helmet).data(),
                        QString("%1").arg(std::get<2>(helmet))
                    );
    item->setText(3, armour_str);
    // points column
    item->setText(6, QString("%1").arg(u->points()));
    switch (u->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        // size column
        item->setText(1, QString("%1").arg(1));
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(u);
        // size column
        item->setText(1, QString("%1").arg(p->size()));
        // command column
        auto command = p->command();
        auto musician_it = command.find(CommandGroup::MUSICIAN);
        auto sb_it = command.find(CommandGroup::STANDARD_BEARER);
        auto champion_it = command.find(CommandGroup::CHAMPION);
        QString command_str("");
        if (musician_it != command.end()) {
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(musician_it->second).data(),
                              QString("%1").arg(std::get<1>(musician_it->second))
                           );
        }
        if (sb_it != command.end()) {
            if (musician_it != command.end()) command_str += QString("\n");
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(sb_it->second).data(),
                              QString("%1").arg(std::get<1>(sb_it->second))
                           );
        }
        if (champion_it != command.end()) {
            if (sb_it != command.end() || musician_it != command.end()) command_str += QString("\n");
            command_str += QString("%1 [%2]").arg(
                              std::get<0>(champion_it->second).data(),
                              QString("%1").arg(std::get<1>(champion_it->second))
                           );
        }
        item->setText(4, command_str);
        break;
    }
    default: break;
    }
    // unit type points column
    switch (u->unit_type()) {
    case armies::UnitType::LORD:
        ui->army_tree->topLevelItem(0)->setText(6, QString("%1").arg(army->lord_points()));
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->setText(6, QString("%1").arg(army->hero_points()));
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->setText(6, QString("%1").arg(army->core_points()));
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->setText(6, QString("%1").arg(army->special_points()));
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->setText(6, QString("%1").arg(army->rare_points()));
        break;
    default: break;
    }
}
