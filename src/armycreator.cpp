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

    ui->army_tree->header()->resizeSection(0, 250); // unit name header
    ui->army_tree->header()->resizeSection(1, 60); // unit size header
    ui->army_tree->header()->resizeSection(2, 180); // weapons header
    ui->army_tree->header()->resizeSection(3, 150); // armour header
    ui->army_tree->header()->resizeSection(4, 180); // command group header
    ui->army_tree->header()->resizeSection(5, 60); // points header
    ui->remove_button->setEnabled(false);
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

// TODO: split into optional_melee_weapon_selected and optional_ranged_weapon_selected
void ArmyCreator::optional_weapon_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    auto rb_name_split = tools::split(rb_name, '_');
    std::string weapon = rb_name_split[0];
    auto from_roster = std::stoi(rb_name_split[3]);
    std::shared_ptr<unit> current;
    if (from_roster) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    if (weapon == "None") {
        switch (current->base_unit_type()) {
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
            // safe to just call remove on both types for now as remove_weapo
            // prevents default weapon from being removed - but will want to
            // change this as it will produce unexpected results for a unit
            // which can take both an optional melee and ranged weapon
            p->remove_weapon(WeaponType::MELEE);
            p->remove_weapon(WeaponType::BALLISTIC);
            break;
        }
        case BaseUnitType::MAGE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
            p->remove_weapon(WeaponType::MELEE);
            p->remove_weapon(WeaponType::BALLISTIC);
            break;
        }
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
            p->remove_weapon(WeaponType::MELEE);
            p->remove_weapon(WeaponType::BALLISTIC);
            break;
        }
        default: break;
        }
        return;
    }
    ItemClass it = static_cast<ItemClass>(std::stoi(rb_name_split[2]));
    // TODO: reset weapon state of roster selected unit to avoid adding multiple
    // weapons when a unit in the selection tree is highlighted and not moved on
    // from when selecting different weapon options
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
        try { p->pick_weapon(it, weapon); } catch (const std::invalid_argument&) { }
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
        try { p->pick_weapon(it, weapon); } catch (const std::invalid_argument&) { }
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        try { p->pick_weapon(it, weapon); } catch (const std::invalid_argument&) { }
        break;
    }
    default: break;
    }
}

void ArmyCreator::optional_armour_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    auto rb_name_split = tools::split(rb_name, '_');
    std::string armour = rb_name_split[0];
    ItemClass it = static_cast<ItemClass>(std::stoi(rb_name_split[2]));
    auto from_roster = std::stoi(rb_name_split[3]);
    std::shared_ptr<unit> current;
    if (from_roster) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
        try { p->pick_armour(it, armour); } catch (const std::invalid_argument&) { }
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
        try { p->pick_armour(it, armour); } catch (const std::invalid_argument&) { }
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        try { p->pick_armour(it, armour); } catch (const std::invalid_argument&) { }
        break;
    }
    default: break;
    }
}

void ArmyCreator::optional_command_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    auto cb_name_split = tools::split(cb_name, '_');
    std::string name = cb_name_split[0];
    CommandGroup member;
    if (cb_name_split[1] == "m") member = CommandGroup::MUSICIAN;
    else if (cb_name_split[1] == "sb") member = CommandGroup::STANDARD_BEARER;
    else member = CommandGroup::CHAMPION;
    auto from_roster = std::stoi(cb_name_split[2]);
    std::shared_ptr<unit> current;
    if (from_roster) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (cb->isChecked()) {
            try {
                p->add_command_member(member);
            } catch (const std::invalid_argument&) {}
        }
        else p->remove_command_member(member);
        break;
    }
    default: break;
    }
}

void ArmyCreator::init_opt_weapons_groupbox(
        QVBoxLayout* vbl,
        const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& opt_weapons,
        const std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>>& weapons,
        bool from_roster,
        int id
     ) {
    QGroupBox* weapons_box = new QGroupBox(tr("Weapons"));
    QVBoxLayout* vbox_weapons = new QVBoxLayout;
    for (const auto& w : opt_weapons) {
        auto tmp = tools::split(std::to_string(std::get<2>(w.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = "";
        if (from_roster)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = w.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(tr(name.data()));
        std::string b_name = w.first + "_" +
                std::to_string(static_cast<int>(std::get<0>(w.second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(w.second))) + "_" +
                ((from_roster) ? "1" : "0") + "_radiobutton";
        b->setObjectName(tr(b_name.data()));
        connect(b, SIGNAL(clicked(bool)), this, SLOT(optional_weapon_selected()));
        vbox_weapons->addWidget(b);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    std::string none_rb_name = std::string("None_optweapon_radiobutton_") + ((from_roster) ? "1" : "0");
    none_rb->setObjectName(tr(none_rb_name.data()));
    none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_weapon_selected()));
    vbox_weapons->addWidget(none_rb);
    vbox_weapons->addStretch(1);
    weapons_box->setLayout(vbox_weapons);
    vbl->addWidget(weapons_box);
}

void ArmyCreator::init_opt_armour_groupbox(
        QVBoxLayout* vbl,
        const std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>>& armour,
        bool from_roster,
        int id
     ) {
    QGroupBox* armours_box = new QGroupBox(tr("Armours"));
    QVBoxLayout* vbox_armours = new QVBoxLayout;
    for (const auto& a : armour) {
        auto tmp = tools::split(std::to_string(std::get<2>(a.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = "";
        if (from_roster)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = a.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(tr(name.data()));
        std::string b_name = a.first + "_" +
                std::to_string(static_cast<int>(std::get<0>(a.second))) + "_" +
                std::to_string(static_cast<int>(std::get<1>(a.second))) + "_" +
                ((from_roster) ? "1" : "0") + "_radiobutton";
        b->setObjectName(tr(b_name.data()));
        connect(b, SIGNAL(clicked(bool)), this, SLOT(optional_armour_selected()));
        vbox_armours->addWidget(b);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    std::string none_rb_name = std::string("None_optarmour_radiobutton_") + ((from_roster) ? "1" : "0");
    none_rb->setObjectName(tr(none_rb_name.data()));
    none_rb->setChecked(true);
    vbox_armours->addWidget(none_rb);
    vbox_armours->addStretch(1);
    armours_box->setLayout(vbox_armours);
    vbl->addWidget(armours_box);
}

void ArmyCreator::init_opt_mounts_groupbox(
        QVBoxLayout* vbl,
        const std::unordered_map<std::string, std::pair<armies::UnitClass, double>>& mounts,
        bool from_roster,
        int id
     ) {
    QGroupBox* mounts_box = new QGroupBox(tr("Mounts"));
    QVBoxLayout* vbox_mounts = new QVBoxLayout;
    for (const auto& m : mounts) {
        auto tmp = tools::split(std::to_string(std::get<1>(m.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = "";
        if (from_roster)
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
    vbl->addWidget(mounts_box);
}

void ArmyCreator::init_opt_extras_groupbox(
        QVBoxLayout* vbl,
        const std::pair<
            std::unordered_map<
                std::string,
                std::pair<bool, double>
            >,
            bool
        >& extras,
        bool from_roster,
        int id
     ) {
    QGroupBox* extras_box = new QGroupBox(tr("Other"));
    QVBoxLayout* vbox_extras = new QVBoxLayout;
    for (const auto& e : extras.first) {
        auto tmp = tools::split(std::to_string(e.second.second), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = "";
        if (from_roster)
            permodel += (st->selected()->base_unit_type() == BaseUnitType::NORMAL &&
                            !(e.second.first)) ? "/model" : "";
        else
            permodel += (army->get_unit(id)->base_unit_type() == BaseUnitType::NORMAL &&
                            !(e.second.first)) ? "/model" : "";
        std::string name = e.first + "(" + pts_str + " pts" + permodel + ")";
        if (extras.second) { // one choice only so use radio buttons
            QRadioButton* rb = new QRadioButton(tr(name.data()));
            rb->setObjectName(tr((e.first + "_radiobutton").data()));
            vbox_extras->addWidget(rb);
        }
        else { // multiple choices allowed so use checkboxes
            QCheckBox* cb = new QCheckBox(tr(name.data()));
            cb->setObjectName(tr((e.first + "_radiobutton").data()));
            vbox_extras->addWidget(cb);
        }
    }
    if (extras.second) {
        QRadioButton* none_rb = new QRadioButton(tr("None"));
        none_rb->setObjectName(tr("None_radiobutton"));
        none_rb->setChecked(true);
        vbox_extras->addWidget(none_rb);
    }
    vbox_extras->addStretch(1);
    extras_box->setLayout(vbox_extras);
    vbl->addWidget(extras_box);
}

QGroupBox* ArmyCreator::init_command_groupbox(
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& opt_command,
        const std::unordered_map<CommandGroup, std::pair<std::string, double>>& command,
        bool from_roster
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
        cb->setObjectName(tr((musician.first + "_m_" + ((from_roster) ? "1" : "0") + "_checkbox").data()));
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
        cb->setObjectName(tr((sb.first + "_sb_" + ((from_roster) ? "1" : "0") + "_checkbox").data()));
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
        cb->setObjectName(tr((champ.first + "_c_" + ((from_roster) ? "1" : "0") + "_checkbox").data()));
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
    std::string sb_name = sb->objectName().toStdString();
    int from_roster = std::stoi(tools::split(sb_name, '_')[1]);
    std::shared_ptr<unit> current;
    if (from_roster) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    {
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        try {
            p->change_size(static_cast<std::size_t>(sb->value()));
        } catch (const std::invalid_argument&) {}
        break;
    }
    default: break;
    }
}

void ArmyCreator::init_size_command_groupbox(
        QVBoxLayout* vbl,
        bool from_roster
     ) {
    QGroupBox* all = new QGroupBox(tr("Size and Command"));
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
    if (from_roster) current = st->selected();
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
    }
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_command = p->handle->optional_command();
        command_buttons = init_command_groupbox(opt_command, p->command(), from_roster);
        size_sb->setMinimum(static_cast<int>(p->min_size()));
        if (p->max_size() == std::numeric_limits<std::size_t>::max())
            size_sb->setMaximum(1000);
        else size_sb->setMaximum(static_cast<int>(p->max_size()));
        std::string size_sb_name = p->name() + "_" + ((from_roster) ? "1" : "0") + "_spinbox";
        size_sb->setObjectName(QString(size_sb_name.data()));
        // set the value of the spinbox to the current unit size if army tree unit is selected
        if (!from_roster) size_sb->setValue(static_cast<int>(p->size()));
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
        delete all;
        return;
    }
    if (command_buttons != nullptr) vbox->addWidget(command_buttons);
    vbox->addStretch(1);
    all->setLayout(vbox);
    vbl->addWidget(all);
}

void ArmyCreator::initialise_unit_options_box(bool from_roster) {
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > opt_weapons;
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > current_weapons;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > opt_armours;
    std::unordered_map<
        std::string,
        std::tuple<ArmourType, ItemClass, double>
    > current_armours;
    std::unordered_map<std::string, std::pair<armies::UnitClass, double>> opt_mounts;
    std::pair<
        std::unordered_map<
            std::string,
            std::pair<bool, double>
        >,
        bool
    > opt_extras;
    int curr_id = 0;
    // overall layout for options box
    QVBoxLayout* vbox = new QVBoxLayout;
    init_size_command_groupbox(vbox, from_roster);
    std::shared_ptr<unit> current;
    if (from_roster) current = st->selected();
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
        opt_extras = p->handle->opt().opt_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
        opt_weapons = p->handle->opt().opt_weapons;
        opt_armours = p->handle->opt().opt_armour;
        opt_mounts = p->handle->opt().opt_mounts;
        opt_extras = p->handle->opt().opt_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
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
        opt_extras = p->handle->opt().opt_extras;
        current_weapons = p->weapons();
        current_armours = p->armour();
        break;
    }
    default:
        break;
    }
    if (!opt_weapons.empty())
        init_opt_weapons_groupbox(vbox, opt_weapons, from_roster, curr_id);
    if (!opt_armours.empty())
        init_opt_armour_groupbox(vbox, opt_armours, from_roster, curr_id);
    if (!opt_mounts.empty())
        init_opt_mounts_groupbox(vbox, opt_mounts, from_roster, curr_id);
    if (!opt_extras.first.empty())
        init_opt_extras_groupbox(vbox, opt_extras, from_roster, curr_id);
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
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        st->change_selection(name);
        ui->add_button->setEnabled(true);
        clear_unit_options_box();
        initialise_unit_options_box(true);
    }
    else
        ui->add_button->setEnabled(false);
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
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        ui->remove_button->setEnabled(true);
        clear_unit_options_box();
        initialise_unit_options_box(false);
    }
    else
        ui->remove_button->setEnabled(false);
}

void ArmyCreator::on_add_button_clicked() {
    st->add_unit_to_army_list(id_counter++);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    // TODO: clean up, reduce duplication
    switch (st->selected()->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<melee_character_unit>(st->selected());
        QVariant v(p->id());
        item->setData(0, Qt::UserRole, v);
        item->setText(0, QString(p->name().data()));
        item->setText(1, QString("%1").arg(1));
        // current weapons
        auto mw = p->melee_weapon();
        auto rw = p->ranged_weapon();
        QString weapons_str("");
        if (!(std::get<1>(mw).empty()))
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(mw).data(),
                               QString("%1").arg(std::get<2>(mw))
                           );
        if (!(std::get<1>(rw).empty())) {
            weapons_str += QString("\n");
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(rw).data(),
                               QString("%1").arg(std::get<2>(rw))
                           );
        }
        item->setText(2, weapons_str);
        // current armour
        auto all_armour = p->armour();
        auto armour = all_armour[ArmourType::ARMOUR];
        auto shield = all_armour[ArmourType::SHIELD];
        auto helmet = all_armour[ArmourType::HELMET];
        QString armour_str("");
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
        item->setText(5, QString("%2").arg(p->points()));
        break;
    }
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(st->selected());
        QVariant v(p->id());
        item->setData(0, Qt::UserRole, v);
        item->setText(0, QString(p->name().data()));
        item->setText(1, QString("%1").arg(1));
        // current weapons
        auto mw = p->melee_weapon();
        auto rw = p->ranged_weapon();
        QString weapons_str("");
        if (!(std::get<1>(mw).empty()))
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(mw).data(),
                               QString("%1").arg(std::get<2>(mw))
                           );
        if (!(std::get<1>(rw).empty())) {
            weapons_str += QString("\n");
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(rw).data(),
                               QString("%1").arg(std::get<2>(rw))
                           );
        }
        item->setText(2, weapons_str);
        // current armour
        auto all_armour = p->armour();
        auto armour = all_armour[ArmourType::ARMOUR];
        auto shield = all_armour[ArmourType::SHIELD];
        auto helmet = all_armour[ArmourType::HELMET];
        QString armour_str("");
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
        item->setText(5, QString("%2").arg(p->points()));
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(st->selected());
        QVariant v(p->id());
        item->setData(0, Qt::UserRole, v);
        item->setText(0, QString(p->name().data()));
        item->setText(1, QString("%1").arg(p->size()));
        // current weapons
        auto mw = p->melee_weapon();
        auto rw = p->ranged_weapon();
        QString weapons_str("");
        if (!(std::get<1>(mw).empty()))
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(mw).data(),
                               QString("%1").arg(std::get<2>(mw))
                           );
        if (!(std::get<1>(rw).empty())) {
            weapons_str += QString("\n");
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(rw).data(),
                               QString("%1").arg(std::get<2>(rw))
                           );
        }
        item->setText(2, weapons_str);
        // current armour
        auto all_armour = p->armour();
        auto armour_it = all_armour.find(ArmourType::ARMOUR);
        auto shield_it = all_armour.find(ArmourType::SHIELD);
        auto helmet_it = all_armour.find(ArmourType::HELMET);
        QString armour_str("");
        if (armour_it != all_armour.end()) {
            armour_str += QString("%1 [%2]").arg(
                              std::get<1>(armour_it->second).data(),
                              QString("%1").arg(std::get<2>(armour_it->second))
                          );
        }
        if (shield_it != all_armour.end()) {
            if (armour_it != all_armour.end()) armour_str += QString("\n");
            armour_str += QString("%1 [%2]").arg(
                              std::get<1>(shield_it->second).data(),
                              QString("%1").arg(std::get<2>(shield_it->second))
                          );
        }
        if (helmet_it != all_armour.end()) {
            if (shield_it != all_armour.end() || armour_it != all_armour.end()) armour_str += QString("\n");
            armour_str += QString("%1 [%2]").arg(
                              std::get<1>(helmet_it->second).data(),
                              QString("%1").arg(std::get<2>(helmet_it->second))
                          );
        }
        item->setText(3, armour_str);
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
        item->setText(5, QString("%2").arg(p->points()));
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(st->selected());
        QVariant v(p->id());
        item->setData(0, Qt::UserRole, v);
        item->setText(0, QString(p->name().data()));
        item->setText(1, QString("%1").arg(p->slave().size()));
        // current weapons
        auto mw = p->slave().melee_weapon();
        auto rw = p->slave().ranged_weapon();
        QString weapons_str("");
        if (!(std::get<1>(mw).empty()))
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(mw).data(),
                               QString("%1").arg(std::get<2>(mw))
                           );
        if (!(std::get<1>(rw).empty())) {
            weapons_str += QString("\n");
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(rw).data(),
                               QString("%1").arg(std::get<2>(rw))
                           );
        }
        item->setText(2, weapons_str);
        // current armour
        auto all_armour = p->slave().armour();
        auto armour = all_armour[ArmourType::ARMOUR];
        auto shield = all_armour[ArmourType::SHIELD];
        auto helmet = all_armour[ArmourType::HELMET];
        QString armour_str("");
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
        item->setText(5, QString("%2").arg(p->points()));
        break;
    }
    default: break;
    }
    // unit type
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
    initialise_unit_options_box(true);
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
        ui->army_tree->topLevelItem(0)->setText(5, QString("%1").arg(army->lord_points()));
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->setText(5, QString("%1").arg(army->hero_points()));
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->setText(5, QString("%1").arg(army->core_points()));
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->setText(5, QString("%1").arg(army->special_points()));
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->setText(5, QString("%1").arg(army->rare_points()));
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
        ui->army_tree->topLevelItem(i)->setText(5, QString("%1").arg(static_cast<double>(0.0)));
    clear_army_tree();
    clear_unit_options_box();
}
