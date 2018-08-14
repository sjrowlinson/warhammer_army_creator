#include "armycreator.h"
#include "ui_armycreator.h"
#include <iostream>
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
    ic_selected = ItemClass::COMMON;
    opt_sel = std::make_shared<option_selector>(st, army);
    // set magic_item_selector to hidden initially
    ui->magic_items_combobox->setHidden(true);
    ui->magic_items_selector->setHidden(true);
    // set army list view header sizes
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::NAME), 250); // unit name header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::SIZE), 60); // unit size header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::WEAPONS), 180); // weapons header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::ARMOUR), 150); // armour header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::COMMAND), 180); // command group header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::EXTRAS), 180); // extras header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::POINTS), 60); // points header
    ui->remove_button->setEnabled(false);
    populate_roster_tree();
}

ArmyCreator::~ArmyCreator() {
    delete ui;
}

void ArmyCreator::populate_roster_tree() {
    auto lords = st->lords();
    std::sort(std::begin(lords), std::end(lords),
        [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); }
    );
    auto heroes = st->heroes();
    std::sort(std::begin(heroes), std::end(heroes),
        [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); }
    );
    auto cores = st->core();
    std::sort(std::begin(cores), std::end(cores),
        [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); }
    );
    auto specials = st->special();
    std::sort(std::begin(specials), std::end(specials),
        [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); }
    );
    auto rares = st->rare();
    std::sort(std::begin(rares), std::end(rares),
        [](const auto& lhs, const auto& rhs) { return lhs->name() < rhs->name(); }
    );
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

// ui element clearers

void ArmyCreator::clear_roster_tree() {
    for (int i = 0; i < 5; ++i)
        ui->roster_tree->topLevelItem(i)->takeChildren();
}

void ArmyCreator::clear_army_tree() {
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->takeChildren();
}

void ArmyCreator::clear_unit_info_box() {
    auto c = ui->unit_info_box->children();
    for (auto& x : c) delete x;
}

void ArmyCreator::clear_unit_options_box() {
    auto c = ui->options_group_box->children();
    for (auto& x : c) delete x;
}

void ArmyCreator::clear_magic_items_selector() {
    ui->magic_items_selector->clear();
    for (auto i = 0; i < ui->magic_items_selector->count(); ++i)
        ui->magic_items_selector->removeTab(i);
}

// option box selection slots

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
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::SIZE);
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

void ArmyCreator::optional_weapon_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_weapon(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::WEAPONS);
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        // TODO: clear and re-initialise just the weapons section of unit options box
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::optional_armour_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_armour(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::ARMOUR);
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::optional_mount_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_mount(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::NAME);
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::optional_command_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    try {
        if (opt_sel->select_command(cb_name, cb->isChecked())) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::COMMAND);
            auto split = tools::split(cb_name, '_');
            if (split[1] == "c" || split[1] == "sb") {
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::WEAPONS);
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::ARMOUR);
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
                clear_unit_options_box();
                initialise_unit_options_box();
            }
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::optional_oco_extra_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_oco_extra(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::optional_mc_extra_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    try {
        if (opt_sel->select_mc_extra(cb_name, cb->isChecked())) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
}

void ArmyCreator::spawn_magic_weapons_window() {
    MagicItemWindow* miw = new MagicItemWindow(ItemType::WEAPON);
    miw->setWindowTitle(tr("Magic Weapon Selection"));
    miw->setFixedWidth(1366);
    miw->setFixedHeight(768);
    miw->show();
    miw->setAttribute(Qt::WA_DeleteOnClose);
}

void ArmyCreator::init_magic_items_selector(std::shared_ptr<unit> current) {
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > mih;
    switch (ic_selected) {
    case ItemClass::COMMON:
        mih = current->base()->common_items_handle();
        break;
    case ItemClass::MAGIC:
        mih = current->base()->magic_items_handle();
        break;
    case ItemClass::FACTION:
        mih = current->base()->faction_items_handle();
        break;
    default: return;
    }
    // magic weapons
    auto magic_weapons_box = setup_magic_weapons_tab(mih->second, current);
    if (magic_weapons_box != nullptr) ui->magic_items_selector->addTab(magic_weapons_box, tr("Magic Weapons"));
    // magic armour
    auto magic_armour_box = setup_magic_armour_tab(mih->second, current);
    if (magic_armour_box != nullptr) ui->magic_items_selector->addTab(magic_armour_box, tr("Magic Armour"));
    ui->magic_items_combobox->setHidden(false);
    ui->magic_items_selector->setHidden(false);
}

// unit info box initialisers

void ArmyCreator::initialise_unit_info_box() {
    std::shared_ptr<unit> u;
    switch (in_tree) {
    case InTree::ARMY:
        u = army->get_unit(ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt());
        break;
    case InTree::ROSTER:
        u = st->selected();
        break;
    default: break;
    }
    // overall layout for unit info box
    QVBoxLayout* vbox = new QVBoxLayout;
    // add points label widget in a frame
    {
        QFrame* f = new QFrame();
        QHBoxLayout* points_label_layout = new QHBoxLayout;
        QLabel* pre_points_label;
        QLabel* points_label;
        switch (u->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(u);
            pre_points_label = new QLabel(tr("Points:"));
            points_label = new QLabel(
                QString(tools::points_str(p->handle_->points(), p->base_unit_type()).data())
            );
            break;
        }
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            pre_points_label = new QLabel(tr("Points per model:"));
            points_label = new QLabel(
                QString(tools::points_str(p->handle->points_per_model(), BaseUnitType::NORMAL).data())
            );
            break;
        }
        default: return;
        }
        points_label_layout->addWidget(pre_points_label);
        points_label_layout->addWidget(points_label);
        f->setLayout(points_label_layout);
        vbox->addWidget(f);

    }
    // add statistics table(s)
    {
        QTableWidget* stats_table = new QTableWidget(2, 9);
        stats_table->horizontalHeader()->setVisible(false);
        stats_table->verticalHeader()->setVisible(false);
        QStringList tmp = {"M", "WS", "BS", "S", "T", "W", "I", "A", "LD"};
        for (auto i = 0; i < tmp.size(); ++i) {
            QTableWidgetItem* entry = new QTableWidgetItem(tmp[i]);
            entry->setFlags(entry->flags() & ~Qt::ItemIsEditable);
            stats_table->setItem(0, i, entry);
        }
        //stats_table->setHorizontalHeaderLabels(tmp);
        switch (u->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(u);
            auto stats = p->handle_->statistics();
            int count = 0;
            for (const auto& x : stats) {
                QTableWidgetItem* entry = new QTableWidgetItem(
                    tr("%1").arg(x)
                );
                entry->setFlags(entry->flags() & ~Qt::ItemIsEditable);
                stats_table->setItem(1, count++, entry);
            }
            break;
        }
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            auto stats = p->handle->statistics();
            int count = 0;
            for (const auto& x : stats) {
                QTableWidgetItem* entry = new QTableWidgetItem(
                    tr("%1").arg(x)
                );
                entry->setFlags(entry->flags() & ~Qt::ItemIsEditable);
                stats_table->setItem(1, count++, entry);
            }
            break;
        }
        default: return;
        }
        stats_table->resizeRowsToContents();
        stats_table->resizeColumnsToContents();
        vbox->addWidget(stats_table);
    }
    // TODO: add list of special rules, probably just as QLabel widget
    {
        QLabel* special_rules_label = new QLabel();
        switch (u->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(u);
            auto sr = p->handle_->special_rules();
            std::string rules_str = "Special Rules: ";
            for (const auto& s : sr)
                rules_str += (s == *std::prev(std::end(sr))) ? s : s + ", ";
            special_rules_label->setTextFormat(Qt::TextFormat::RichText);
            special_rules_label->setText(tr(rules_str.data()));
            break;
        }
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            auto sr = p->handle->special_rules();
            std::string rules_str = "Special Rules: ";
            for (const auto& s : sr)
                rules_str += (s == *std::prev(std::end(sr))) ? s : s + ", ";
            special_rules_label->setText(tr(rules_str.data()));
            break;
        }
        default: return;
        }
        vbox->addWidget(special_rules_label);
    }
    ui->unit_info_box->setLayout(vbox);
}

QGroupBox* ArmyCreator::setup_magic_weapons_tab(const std::unordered_map<std::string, item>& items,
                                                std::shared_ptr<unit> current) {
    if (items.empty()) return nullptr;
    auto weapons = tools::magic_items_of(items, ItemType::WEAPON);
    if (!std::count_if(weapons.begin(), weapons.end(), [&current](const auto& x) {
            return x.second.allowed_units.empty() || x.second.allowed_units.count(current->name());
        })) return nullptr;
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    // melee box
    QGroupBox* mbox = new QGroupBox(tr("Melee"));
    QHBoxLayout* mhlayout = new QHBoxLayout;
    // ranged box
    QGroupBox* rbox = new QGroupBox(tr("Ranged"));
    QHBoxLayout* rhlayout = new QHBoxLayout;
    for (const auto& w : weapons) {
        if (w.second.allowed_units.size() && !w.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
            if (w.second.points > p->handle->magic_item_budget()) continue;
            break;
        }
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
            if (w.second.points > p->handle->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(w.second.points);
        QRadioButton* rb = new QRadioButton(tr((w.first + " (" + pts_str + " pts)").data()));
        rb->setToolTip(tr(w.second.description.data()));
        if (w.second.weapon_type == WeaponType::MELEE) mhlayout->addWidget(rb);
        else if (w.second.weapon_type == WeaponType::BALLISTIC) rhlayout->addWidget(rb);
    }
    // melee none button
    QRadioButton* none_rb_m = new QRadioButton(tr("None"));
    none_rb_m->setChecked(true);
    mhlayout->addWidget(none_rb_m);
    mhlayout->addStretch(1);
    mbox->setLayout(mhlayout);
    // ranged none button
    QRadioButton* none_rb_r = new QRadioButton(tr("None"));
    none_rb_r->setChecked(true);
    rhlayout->addWidget(none_rb_r);
    rhlayout->addStretch(1);
    rbox->setLayout(rhlayout);
    // add to overall weapons box
    vlayout->addWidget(mbox);
    vlayout->addWidget(rbox);
    vlayout->addStretch(1);
    box->setLayout(vlayout);
    return box;
}

QGroupBox* ArmyCreator::setup_magic_armour_tab(const std::unordered_map<std::string, item>& items,
                                                std::shared_ptr<unit> current) {
    auto armour = tools::magic_items_of(items, ItemType::ARMOUR);
    if (!std::count_if(armour.begin(), armour.end(), [&current](const auto& x) {
            return x.second.allowed_units.empty() || x.second.allowed_units.count(current->name());
        })) return nullptr;
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    // armour
    QGroupBox* abox = new QGroupBox(tr("Body Armour"));
    QHBoxLayout* ahlayout = new QHBoxLayout;
    // shield
    QGroupBox* sbox = new QGroupBox(tr("Shield"));
    QHBoxLayout* shlayout = new QHBoxLayout;
    // helmet
    QGroupBox* hbox = new QGroupBox(tr("Helmet"));
    QHBoxLayout* hhlayout = new QHBoxLayout;
    for (const auto& a : armour) {
        if (a.second.allowed_units.size() && !a.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
            if (a.second.points > p->handle->magic_item_budget()) continue;
            break;
        }
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<melee_character_unit>(current);
            if (a.second.points > p->handle->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(a.second.points);
        QRadioButton* rb = new QRadioButton(tr((a.first + " (" + pts_str + " pts)").data()));
        rb->setToolTip(tr(a.second.description.data()));
        switch (a.second.armour_type) {
        case ArmourType::ARMOUR:
            ahlayout->addWidget(rb);
            break;
        case ArmourType::SHIELD:
            shlayout->addWidget(rb);
            break;
        case ArmourType::HELMET:
            hhlayout->addWidget(rb);
            break;
        default: break;
        }
    }
    // armour
    QRadioButton* none_rb_a = new QRadioButton(tr("None"));
    none_rb_a->setChecked(true);
    ahlayout->addWidget(none_rb_a);
    ahlayout->addStretch(1);
    abox->setLayout(ahlayout);
    // shield
    QRadioButton* none_rb_s = new QRadioButton(tr("None"));
    none_rb_s->setChecked(true);
    shlayout->addWidget(none_rb_s);
    shlayout->addStretch(1);
    sbox->setLayout(shlayout);
    // helmet
    QRadioButton* none_rb_h = new QRadioButton(tr("None"));
    none_rb_h->setChecked(true);
    hhlayout->addWidget(none_rb_h);
    hhlayout->addStretch(1);
    hbox->setLayout(hhlayout);
    // finish up
    vlayout->addWidget(abox);
    vlayout->addWidget(sbox);
    vlayout->addWidget(hbox);
    vlayout->addStretch(1);
    box->setLayout(vlayout);
    return box;
}

// unit options box initialisers

void ArmyCreator::initialise_unit_options_box() {
    std::shared_ptr<unit> current;
    int curr_id = 0;
    switch (in_tree) {
    case InTree::ARMY:
        curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        current = army->get_unit(curr_id);
        break;
    case InTree::ROSTER:
        current = st->selected();
        break;
    default: return;
    }
    QVBoxLayout* vbox = new QVBoxLayout; // overall layout for options box
    // set-up optional size and command box
    auto sc_box = initialise_size_command_groupbox(current);
    if (sc_box != nullptr) vbox->addWidget(sc_box);
    // set-up optional weapons boxes
    auto weapons_boxes = initialise_opt_weapons_groupbox(current);
    if (weapons_boxes.first != nullptr) vbox->addWidget(weapons_boxes.first);
    if (weapons_boxes.second != nullptr) vbox->addWidget(weapons_boxes.second);
    // set-up optional armour boxes
    auto armour_boxes = initialise_opt_armour_groupbox(current);
    if (armour_boxes.first != nullptr) vbox->addWidget(armour_boxes.first);
    if (armour_boxes.second != nullptr) vbox->addWidget(armour_boxes.second);
    // set-up optional mount box
    auto mount_box = initialise_opt_mounts_groupbox(current);
    if (mount_box != nullptr) vbox->addWidget(mount_box);
    // set-up optional extras boxes
    auto extras_boxes = initialise_opt_extras_groupbox(current);
    if (extras_boxes.first != nullptr) vbox->addWidget(extras_boxes.first);
    if (extras_boxes.second != nullptr) vbox->addWidget(extras_boxes.second);
    ui->options_group_box->setLayout(vbox);
}

QGroupBox* ArmyCreator::initialise_size_command_groupbox(std::shared_ptr<unit> current) {
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
        if (current->unit_class() == armies::UnitClass::MONSTER) return nullptr;
        break;
    case BaseUnitType::MIXED:
        // TODO: change to break; once mixed unit implementation complete
        return nullptr;
    default: return nullptr;
    }
    QGroupBox* sc_groupbox = new QGroupBox(tr("Size and Command Group"));
    QVBoxLayout* sc_box_layout = new QVBoxLayout;
    QHBoxLayout* size_box_layout = new QHBoxLayout;
    // unit size
    QLabel* label = new QLabel(tr("Unit size"));
    QSpinBox* size_sb = new QSpinBox();
    // command group
    QGroupBox* command_box = nullptr;
    if (current->base_unit_type() == BaseUnitType::NORMAL) {
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
        connect(size_sb, SIGNAL(valueChanged(int)), this, SLOT(change_unit_size()));
        size_box_layout->addWidget(label);
        size_box_layout->addWidget(size_sb);
        size_box_layout->addStretch(1);
        sc_box_layout->addLayout(size_box_layout);
    }
    else { // BaseUnitType::MIXED
        // TODO: implement
    }
    command_box = initialise_command_groupbox(current);
    if (command_box != nullptr) sc_box_layout->addWidget(command_box);
    sc_box_layout->addStretch(1);
    sc_groupbox->setLayout(sc_box_layout);
    return sc_groupbox;
}

QGroupBox* ArmyCreator::initialise_command_groupbox(std::shared_ptr<unit> current) {
    std::unordered_map<CommandGroup, std::pair<std::string, double>> opt_command;
    std::unordered_map<CommandGroup, std::pair<std::string, double>> command;
    switch (current->base_unit_type()) {
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        opt_command = p->handle->optional_command();
        if (in_tree == InTree::ARMY) command = p->command();
        break;
    }
    default: return nullptr;
    }
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
        cb->setObjectName(QString((musician.first + "_m_checkbox").data()));
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
        cb->setObjectName(QString((sb.first + "_sb_checkbox").data()));
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
        cb->setObjectName(QString((champ.first + "_c_checkbox").data()));
        if (command.count(CommandGroup::CHAMPION)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_command_selected()));
        vb->addWidget(cb);
    }
    vb->addStretch(1);
    gb->setLayout(vb);
    return gb;
}

std::pair<QGroupBox*, QGroupBox*> ArmyCreator::initialise_opt_weapons_groupbox(std::shared_ptr<unit> current) {
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
        QGroupBox* weapons_box = new QGroupBox(tr("Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = initialise_opt_weapons_subgroupbox(WeaponType::MELEE, current, false);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = initialise_opt_weapons_subgroupbox(WeaponType::BALLISTIC, current, false);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.first = weapons_box;
    }
    if (has_champ_opt_weapons) {
        QGroupBox* weapons_box = new QGroupBox(tr("Champion Weapons"));
        QVBoxLayout* weapons_box_layout = new QVBoxLayout;
        auto melee_subbox = initialise_opt_weapons_subgroupbox(WeaponType::MELEE, current, true);
        if (melee_subbox != nullptr) weapons_box_layout->addWidget(melee_subbox);
        auto ranged_subbox = initialise_opt_weapons_subgroupbox(WeaponType::BALLISTIC, current, true);
        if (ranged_subbox != nullptr) weapons_box_layout->addWidget(ranged_subbox);
        weapons_box_layout->addStretch(1);
        weapons_box->setLayout(weapons_box_layout);
        boxes.second = weapons_box;
    }
    return boxes;
}

QGroupBox* ArmyCreator::initialise_opt_weapons_subgroupbox(
    WeaponType wt, std::shared_ptr<unit> current, bool champion
) {
    std::unordered_map<std::string, std::tuple<WeaponType, ItemClass, double>> opt_weapons;
    std::unordered_map<WeaponType, std::tuple<ItemClass, std::string, double>> curr_weapons;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_weapons = p->handle_->opt().opt_weapons;
        if (in_tree == InTree::ARMY) curr_weapons = p->weapons();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_weapons = p->handle->champion_opt().opt_weapons;
            p->switch_model_select(ModelSelect::CHAMPION);
            if (in_tree == InTree::ARMY) curr_weapons = p->weapons();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_weapons = p->handle->opt().opt_weapons;
            p->switch_model_select(ModelSelect::DEFAULT);
            if (in_tree == InTree::ARMY) curr_weapons = p->weapons();
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
    QGroupBox* weapons_subbox = new QGroupBox(tr(subbox_label.data()));
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
        QRadioButton* rb = new QRadioButton(tr(button_label.data()));
        rb->setObjectName(QString(button_name.data()));
        // check if current unit weapon map contains this weapon
        if (curr_weapons.count(std::get<0>(w->second))) {
            if (std::get<1>(curr_weapons.at(std::get<0>(w->second))) == w->first) {
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
        none_rb_name += std::string("melee_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    case WeaponType::BALLISTIC:
        none_rb_name += std::string("ranged_") + ((champion) ? "champion" : "default") + "_radiobutton";
        break;
    default: throw std::runtime_error("Unrecognised weapon type!");
    }
    none_rb->setObjectName(QString(none_rb_name.data()));
    if (!has_weapon) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_weapon_selected()));
    weapons_subbox_layout->addWidget(none_rb);
    weapons_subbox_layout->addStretch(1);
    weapons_subbox->setLayout(weapons_subbox_layout);
    return weapons_subbox;
}

std::pair<QGroupBox*, QGroupBox*> ArmyCreator::initialise_opt_armour_groupbox(std::shared_ptr<unit> current) {
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
        QGroupBox* armour_box = new QGroupBox(tr("Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = initialise_opt_armour_subgroupbox(ArmourType::ARMOUR, current, false);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = initialise_opt_armour_subgroupbox(ArmourType::SHIELD, current, false);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = initialise_opt_armour_subgroupbox(ArmourType::HELMET, current, false);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.first = armour_box;
    }
    if (has_champ_opt_armour) {
        QGroupBox* armour_box = new QGroupBox(tr("Champion Armour"));
        QVBoxLayout* armour_box_layout = new QVBoxLayout;
        auto body_subbox = initialise_opt_armour_subgroupbox(ArmourType::ARMOUR, current, true);
        if (body_subbox != nullptr) armour_box_layout->addWidget(body_subbox);
        auto shield_subbox = initialise_opt_armour_subgroupbox(ArmourType::SHIELD, current, true);
        if (shield_subbox != nullptr) armour_box_layout->addWidget(shield_subbox);
        auto helmet_subbox = initialise_opt_armour_subgroupbox(ArmourType::HELMET, current, true);
        if (helmet_subbox != nullptr) armour_box_layout->addWidget(helmet_subbox);
        armour_box_layout->addStretch(1);
        armour_box->setLayout(armour_box_layout);
        boxes.second = armour_box;
    }
    return boxes;
}

QGroupBox* ArmyCreator::initialise_opt_armour_subgroupbox(
    ArmourType at, std::shared_ptr<unit> current, bool champion
) {
    std::unordered_map<std::string, std::tuple<ArmourType, ItemClass, double>> opt_armour;
    std::unordered_map<ArmourType, std::tuple<ItemClass, std::string, double>> curr_armour;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        opt_armour = p->handle_->opt().opt_armour;
        if (in_tree == InTree::ARMY) curr_armour = p->armour();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        if (champion) {
            opt_armour = p->handle->champion_opt().opt_armour;
            p->switch_model_select(ModelSelect::CHAMPION);
            if (in_tree == InTree::ARMY) curr_armour = p->armour();
            p->switch_model_select(ModelSelect::DEFAULT);
        }
        else {
            opt_armour = p->handle->opt().opt_armour;
            p->switch_model_select(ModelSelect::DEFAULT);
            if (in_tree == InTree::ARMY) curr_armour = p->armour();
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
    QGroupBox* armour_subbox = new QGroupBox(tr(subbox_label.data()));
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
        QRadioButton* rb = new QRadioButton(tr(button_label.data()));
        rb->setObjectName(QString(button_name.data()));
        // check if current unit armour map contains this piece of armour
        if (curr_armour.count(std::get<0>(a->second))) {
               if (std::get<1>(curr_armour.at(std::get<0>(a->second))) == a->first) {
                   rb->setChecked(true);
                   has_armour = true;
               }
        }
        connect(rb, SIGNAL(clicked(bool)), this, SLOT(optional_armour_selected()));
        armour_subbox_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
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
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_armour_selected()));
    armour_subbox_layout->addWidget(none_rb);
    armour_subbox_layout->addStretch(1);
    armour_subbox->setLayout(armour_subbox_layout);
    return armour_subbox;
}

QGroupBox* ArmyCreator::initialise_opt_mounts_groupbox(std::shared_ptr<unit> current) {
    std::unordered_map<std::string, std::pair<armies::UnitClass, double>> opt_mounts;
    std::pair<std::string, std::pair<armies::UnitClass, double>> mount;
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
    QGroupBox* mounts_box = new QGroupBox(tr("Mounts"));
    QVBoxLayout* vbox_mounts = new QVBoxLayout;
    bool has_mount = false;
    for (const auto& m : opt_mounts) {
        auto tmp = tools::split(std::to_string(std::get<1>(m.second)), '.');
        for (auto& s : tmp) tools::remove_leading_whitespaces(s);
        std::string pts_str = (tools::starts_with(tmp[1], '0')) ? tmp[0] : tmp[0] + "." + tmp[1].substr(0, 1);
        std::string permodel = (current->base_unit_type() == BaseUnitType::NORMAL) ? "/model" : "";
        std::string name = m.first + " (" + pts_str + " pts" + permodel + ")";
        QRadioButton* b = new QRadioButton(tr(name.data()));
        b->setObjectName(QString((m.first + "_radiobutton").data()));
        if (mount.first == m.first) {
            b->setChecked(true);
            has_mount = true;
        }
        connect(b, SIGNAL(clicked(bool)), this, SLOT(optional_mount_selected()));
        vbox_mounts->addWidget(b);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    none_rb->setObjectName(QString("None_mounts_radiobutton"));
    if (!has_mount) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_mount_selected()));
    vbox_mounts->addWidget(none_rb);
    vbox_mounts->addStretch(1);
    mounts_box->setLayout(vbox_mounts);
    return mounts_box;
}

std::pair<QGroupBox*, QGroupBox*> ArmyCreator::initialise_opt_extras_groupbox(std::shared_ptr<unit> current) {
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
        QGroupBox* extras_box = new QGroupBox(tr("Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_opt_oco_extras) {
            auto oco_subbox = initialise_opt_oco_extras_groupbox(current, false);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_opt_mc_extras) {
            auto mc_subbox = initialise_opt_mc_extras_groupbox(current, false);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.first = extras_box;
    }
    if (has_champ_opt_oco_extras || has_champ_opt_mc_extras) {
        QGroupBox* extras_box = new QGroupBox(tr("Champion Other"));
        QVBoxLayout* extras_box_layout = new QVBoxLayout;
        if (has_champ_opt_oco_extras) {
            auto oco_subbox = initialise_opt_oco_extras_groupbox(current, true);
            extras_box_layout->addWidget(oco_subbox);
        }
        if (has_champ_opt_mc_extras) {
            auto mc_subbox = initialise_opt_mc_extras_groupbox(current, true);
            extras_box_layout->addWidget(mc_subbox);
        }
        extras_box_layout->addStretch(1);
        extras_box->setLayout(extras_box_layout);
        boxes.second = extras_box;
    }
    return boxes;
}

QGroupBox* ArmyCreator::initialise_opt_oco_extras_groupbox(std::shared_ptr<unit> current, bool champion) {
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
        QRadioButton* rb = new QRadioButton(tr(label.data()));
        rb->setObjectName(QString(name.data()));
        if (curr_oco_extra.first == e.first) {
            rb->setChecked(true);
            has_extra = true;
        }
        connect(rb, SIGNAL(clicked(bool)), this, SLOT(optional_oco_extra_selected()));
        oco_box_layout->addWidget(rb);
    }
    QRadioButton* none_rb = new QRadioButton(tr("None"));
    std::string none_rb_name = std::string("None_") + ((champion) ? "champion" : "default") + "_ocoextra_radiobutton";
    none_rb->setObjectName(QString(none_rb_name.data()));
    if (!has_extra) none_rb->setChecked(true);
    connect(none_rb, SIGNAL(clicked(bool)), this, SLOT(optional_oco_extra_selected()));
    oco_box_layout->addWidget(none_rb);
    oco_box_layout->addStretch(1);
    oco_box->setLayout(oco_box_layout);
    return oco_box;
}

QGroupBox* ArmyCreator::initialise_opt_mc_extras_groupbox(std::shared_ptr<unit> current, bool champion) {
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
        QCheckBox* cb = new QCheckBox(tr(label.data()));
        cb->setObjectName(QString(name.data()));
        if (curr_mc_extras.count(e.first)) cb->setChecked(true);
        connect(cb, SIGNAL(clicked(bool)), this, SLOT(optional_mc_extra_selected()));
        mc_box_layout->addWidget(cb);
    }
    mc_box_layout->addStretch(1);
    mc_box->setLayout(mc_box_layout);
    return mc_box;
}

void ArmyCreator::on_actionExit_triggered() {
    QCoreApplication::quit();
}

void ArmyCreator::on_faction_combobox_currentTextChanged(const QString& faction) {
    race = armies::s_map_string_faction[faction.toStdString()];
    army->clear();
    ui->current_pts_label->setText(QString("%1").arg(static_cast<double>(0.0)));
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->setText(6, QString("%1").arg(static_cast<double>(0.0)));
    st->reset(race, *army);
    id_counter = 0;
    clear_roster_tree();
    clear_army_tree();
    clear_unit_info_box();
    clear_unit_options_box();
    clear_magic_items_selector();
    // set us to NEITHER tree to avoid attempting to get the current
    // selected unitt of this->st when roster_tree->currentItem will
    // no longer point to a valid unit type
    in_tree = InTree::NEITHER;
    ui->magic_items_combobox->clear();
    populate_roster_tree();
    ui->magic_items_combobox->addItem(QString("Common"), QVariant(3));
    if (!st->magic_items_name().empty())
        ui->magic_items_combobox->addItem(QString(st->magic_items_name().data()), QVariant(1));
    if (!st->faction_items_name().empty())
        ui->magic_items_combobox->addItem(QString(st->faction_items_name().data()), QVariant(2));
    ui->magic_items_combobox->hide();
}

void ArmyCreator::on_magic_items_combobox_currentTextChanged(const QString& ic_select) {
    (void)(ic_select);
    auto v = ui->magic_items_combobox->currentData().toInt();
    ic_selected = static_cast<ItemClass>(v);
    clear_magic_items_selector();
    std::shared_ptr<unit> current;
    switch (in_tree) {
    case InTree::ARMY:
        current = army->get_unit(ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt());
        break;
    case InTree::ROSTER:
        current = st->selected();
        break;
    default: return;
    }
    init_magic_items_selector(current);
}

void ArmyCreator::on_pts_limit_spinbox_valueChanged(double pts) {
    army->change_points_limit(pts);
}

// tree item changed or activated slots

void ArmyCreator::on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    ui->army_tree->setCurrentItem(ui->army_tree->topLevelItem(0));
    ui->army_tree->clearSelection();
    ui->roster_tree->setCurrentItem(current);
    clear_unit_info_box();
    clear_unit_options_box();
    clear_magic_items_selector();
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        st->change_selection(name);
        in_tree = InTree::ROSTER;
        ui->add_button->setEnabled(true);
        initialise_unit_info_box();
        initialise_unit_options_box();
        opt_sel->reset(st->selected(), in_tree);
        switch (st->selected()->unit_type()) {
        case armies::UnitType::LORD:
        case armies::UnitType::HERO:
        {
            init_magic_items_selector(st->selected());
            break;
        }
        default:
            ui->magic_items_combobox->hide();
            ui->magic_items_selector->hide();
            break;
        }
    }
    else {
        ui->add_button->setEnabled(false);
    }
    ui->duplicate_button->setEnabled(false);
    ui->remove_button->setEnabled(false);
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
    ui->roster_tree->setCurrentItem(ui->roster_tree->topLevelItem(0));
    ui->roster_tree->clearSelection();
    ui->army_tree->setCurrentItem(current);
    clear_unit_info_box();
    clear_unit_options_box();
    clear_magic_items_selector();
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        in_tree = InTree::ARMY;
        ui->duplicate_button->setEnabled(true);
        ui->remove_button->setEnabled(true);
        initialise_unit_info_box();
        initialise_unit_options_box();
        int id = current->data(0, Qt::UserRole).toInt();
        opt_sel->reset(army->get_unit(id), in_tree);
        switch (army->get_unit(id)->unit_type()) {
        case armies::UnitType::LORD:
        case armies::UnitType::HERO:
        {
            init_magic_items_selector(army->get_unit(id));
            break;
        }
        default:
            ui->magic_items_selector->hide();
            break;
        }
    }
    else {
        ui->duplicate_button->setEnabled(false);
        ui->remove_button->setEnabled(false);
    }
}

// army_tree modifying button slots

void ArmyCreator::on_add_button_clicked() {
    st->add_unit_to_army_list(id_counter++);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    std::shared_ptr<unit> u = st->selected();
    // assign id
    QVariant v(u->id());
    item->setData(0, Qt::UserRole, v);
    update_unit_display(item, true);
    st->change_selection(u->name());
    clear_unit_options_box();
    // just make sure to keep us in roster tree
    // in case it gets changed somehow
    in_tree = InTree::ROSTER;
    initialise_unit_options_box();
}

void ArmyCreator::on_duplicate_button_clicked() {
    QTreeWidgetItem* curr_item = ui->army_tree->currentItem();
    auto u = army->get_unit(curr_item->data(0, Qt::UserRole).toInt());
    std::shared_ptr<unit> cpy_ptr;
    switch (u->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(u);
        cpy_ptr = std::make_shared<mage_character_unit>(mage_character_unit(*p));
        break;
    }
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<melee_character_unit>(u);
        cpy_ptr = std::make_shared<melee_character_unit>(melee_character_unit(*p));
        break;
    }
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(u);
        cpy_ptr = std::make_shared<normal_unit>(normal_unit(*p));
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(u);
        cpy_ptr = std::make_shared<mixed_unit>(mixed_unit(*p));
        break;
    }
    default: return;
    }
    cpy_ptr->set_id(id_counter++);
    army->add_unit(cpy_ptr);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    QVariant v(cpy_ptr->id());
    item->setData(0, Qt::UserRole, v);
    update_unit_display(item, true, ArmyTreeColumn::ALL, true);
    clear_unit_options_box();
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

void ArmyCreator::update_unit_display(
        QTreeWidgetItem* item,
        bool adding,
        ArmyTreeColumn column,
        bool copying
    ) {
    std::shared_ptr<unit> u;
    if (adding) {
        if (copying) u = army->get_unit(item->data(0, Qt::UserRole).toInt());
        else u = st->selected();
    }
    else u = army->get_unit(item->data(0, Qt::UserRole).toInt());
    switch (column) {
    case ArmyTreeColumn::NAME:
    {
        std::string name = u->name() + ((u->mount().first.empty()) ? "" : "\n(" + u->mount().first + ")");
        item->setText(static_cast<int>(column), QString(name.data()));
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    }
    case ArmyTreeColumn::SIZE:
        switch (u->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
            item->setText(static_cast<int>(column), QString("%1").arg(1));
            break;
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            item->setText(static_cast<int>(column), QString("%1").arg(p->size()));
            break;
        }
        case BaseUnitType::MIXED:
        {
            auto p = std::dynamic_pointer_cast<mixed_unit>(u);
            item->setText(static_cast<int>(column), QString("%1").arg(p->slave().size()));
            break;
        }
        default: break;
        }
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    case ArmyTreeColumn::WEAPONS:
    {
        auto weapons = u->weapons();
        QString weapons_str;
        auto melee_weapon = weapons.find(WeaponType::MELEE);
        auto ranged_weapon = weapons.find(WeaponType::BALLISTIC);
        if (melee_weapon != weapons.end() && !(std::get<1>(melee_weapon->second).empty()))
            weapons_str += QString("%1 [%2]").arg(
                               std::get<1>(melee_weapon->second).data(),
                               QString("%1").arg(std::get<2>(melee_weapon->second))
                           );
        if (ranged_weapon != weapons.end() && !(std::get<1>(ranged_weapon->second).empty()))
            weapons_str += QString("\n") + QString("%1 [%2]").arg(
                               std::get<1>(ranged_weapon->second).data(),
                               QString("%1").arg(std::get<2>(ranged_weapon->second))
                           );
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto champ_weapons = u->weapons();
            auto champ_melee_weapon = champ_weapons.find(WeaponType::MELEE);
            auto champ_ranged_weapon = champ_weapons.find(WeaponType::BALLISTIC);
            if (champ_melee_weapon != champ_weapons.end() &&
                    !(std::get<1>(champ_melee_weapon->second).empty() ||
                      std::get<1>(champ_melee_weapon->second) == "Hand weapon"))
                weapons_str += QString("\n%1 [%2]").arg(
                                   (std::get<1>(champ_melee_weapon->second) + " (Champion)").data(),
                                   QString("%1").arg(std::get<2>(champ_melee_weapon->second))
                               );
            if (champ_ranged_weapon != champ_weapons.end() && !(std::get<1>(champ_ranged_weapon->second).empty()))
                weapons_str += QString("\n") + QString("%1 [%2]").arg(
                                   (std::get<1>(champ_ranged_weapon->second) + " (Champion)").data(),
                                   QString("%1").arg(std::get<2>(champ_ranged_weapon->second))
                               );
        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(static_cast<int>(column), weapons_str);
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    }
    case ArmyTreeColumn::ARMOUR:
    {
        QString armour_str;
        auto all_armour = u->armour();
        auto armour = all_armour.find(ArmourType::ARMOUR);
        auto shield = all_armour.find(ArmourType::SHIELD);
        auto helmet = all_armour.find(ArmourType::HELMET);
        if (armour != all_armour.end() && !(std::get<1>(armour->second).empty()))
            armour_str += QString("%1 [%2]").arg(
                            std::get<1>(armour->second).data(),
                            QString("%1").arg(std::get<2>(armour->second))
                        );
        if (shield != all_armour.end() && !(std::get<1>(shield->second).empty())) {
            if (!armour_str.isEmpty()) armour_str += QString("\n");
            armour_str += QString("%1 [%2]").arg(
                            std::get<1>(shield->second).data(),
                            QString("%1").arg(std::get<2>(shield->second))
                        );
        }
        if (helmet != all_armour.end() && !(std::get<1>(helmet->second).empty()))
            armour_str += QString("%1 [%2]").arg(
                            std::get<1>(helmet->second).data(),
                            QString("%1").arg(std::get<2>(helmet->second))
                        );
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto champ_all_armour = u->armour();
            auto champ_armour = champ_all_armour.find(ArmourType::ARMOUR);
            auto champ_shield = champ_all_armour.find(ArmourType::SHIELD);
            auto champ_helmet = champ_all_armour.find(ArmourType::HELMET);
            if (champ_armour != champ_all_armour.end() && !(std::get<1>(champ_armour->second).empty()))
                armour_str += QString("\n%1 [%2]").arg(
                                (std::get<1>(champ_armour->second) + " (Champion)").data(),
                                QString("%1").arg(std::get<2>(champ_armour->second))
                            );
            if (champ_shield != champ_all_armour.end() && !(std::get<1>(champ_shield->second).empty()))
                armour_str += QString("%1 [%2]").arg(
                                (std::get<1>(champ_shield->second) + " (Champion)").data(),
                                QString("%1").arg(std::get<2>(champ_shield->second))
                            );
            if (champ_helmet != champ_all_armour.end() && !(std::get<1>(champ_helmet->second).empty()))
                armour_str += QString("%1 [%2]").arg(
                                (std::get<1>(champ_helmet->second) + " (Champion)").data(),
                                QString("%1").arg(std::get<2>(champ_helmet->second))
                            );
        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(static_cast<int>(column), armour_str);
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    }
    case ArmyTreeColumn::COMMAND:
    {
        switch (u->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
            break;
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            auto command = p->command();
            auto musician_it = command.find(CommandGroup::MUSICIAN);
            auto sb_it = command.find(CommandGroup::STANDARD_BEARER);
            auto champion_it = command.find(CommandGroup::CHAMPION);
            QString command_str;
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
            item->setText(static_cast<int>(column), command_str);
            break;
        }
        case BaseUnitType::MIXED:
        {
            break;
        }
        default: break;
        }
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    }
    case ArmyTreeColumn::EXTRAS:
    {
        QString extras_str;
        auto oco_extra = u->oco_extra();
        if (!(oco_extra.first.empty()))
            extras_str += QString("%1 [%2]").arg(
                              oco_extra.first.data(),
                              QString("%1").arg(oco_extra.second.second)
                          );
        auto mc_extras = u->mc_extras();
        if (!mc_extras.empty() && !oco_extra.first.empty()) extras_str += QString("\n");
        for (auto it = std::begin(mc_extras); it != std::end(mc_extras); ++it) {
            extras_str += QString("%1 [%2]").arg(
                                      it->first.data(),
                                      QString("%1").arg(it->second.second)
                                  );
            if (++it != std::end(mc_extras)) extras_str += QString("\n");
            --it;
        }
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto champ_oco_extra = u->oco_extra();
            if (!(champ_oco_extra.first.empty()))
                extras_str += QString("\n%1 [%2]").arg(
                                  (champ_oco_extra.first + " (Champion)").data(),
                                  QString("%1").arg(champ_oco_extra.second.second)
                              );
            auto champ_mc_extras = u->mc_extras();
            if (!champ_mc_extras.empty() && !champ_oco_extra.first.empty()) extras_str += QString("\n");
            for (auto it = std::begin(champ_mc_extras); it != std::end(champ_mc_extras); ++it) {
                extras_str += QString("%1 [%2]").arg(
                                          (it->first + " (Champion)").data(),
                                          QString("%1").arg(it->second.second)
                                      );
                if (++it != std::end(champ_mc_extras)) extras_str += QString("\n");
                --it;
            }

        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(static_cast<int>(column), extras_str);
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        break;
    }
    case ArmyTreeColumn::POINTS:
    {
        item->setText(static_cast<int>(column), QString("%1").arg(u->points()));
        break;
    }
    case ArmyTreeColumn::ALL:
    {
        update_unit_display(item, adding, ArmyTreeColumn::NAME, copying);
        update_unit_display(item, adding, ArmyTreeColumn::SIZE, copying);
        update_unit_display(item, adding, ArmyTreeColumn::WEAPONS, copying);
        update_unit_display(item, adding, ArmyTreeColumn::ARMOUR, copying);
        update_unit_display(item, adding, ArmyTreeColumn::COMMAND, copying);
        update_unit_display(item, adding, ArmyTreeColumn::EXTRAS, copying);
        update_unit_display(item, adding, ArmyTreeColumn::POINTS, copying);
        if (adding) {
            int pts_col = static_cast<int>(ArmyTreeColumn::POINTS);
            switch (u->unit_type()) {
            case armies::UnitType::LORD:
                ui->army_tree->topLevelItem(0)->addChild(item);
                ui->army_tree->topLevelItem(0)->setText(pts_col, QString("%1").arg(army->lord_points()));
                ui->army_tree->topLevelItem(0)->setExpanded(true);
                break;
            case armies::UnitType::HERO:
                ui->army_tree->topLevelItem(1)->addChild(item);
                ui->army_tree->topLevelItem(1)->setText(pts_col, QString("%1").arg(army->hero_points()));
                ui->army_tree->topLevelItem(1)->setExpanded(true);
                break;
            case armies::UnitType::CORE:
                ui->army_tree->topLevelItem(2)->addChild(item);
                ui->army_tree->topLevelItem(2)->setText(pts_col, QString("%1").arg(army->core_points()));
                ui->army_tree->topLevelItem(2)->setExpanded(true);
                break;
            case armies::UnitType::SPECIAL:
                ui->army_tree->topLevelItem(3)->addChild(item);
                ui->army_tree->topLevelItem(3)->setText(pts_col, QString("%1").arg(army->special_points()));
                ui->army_tree->topLevelItem(3)->setExpanded(true);
                break;
            case armies::UnitType::RARE:
                ui->army_tree->topLevelItem(4)->addChild(item);
                ui->army_tree->topLevelItem(4)->setText(pts_col, QString("%1").arg(army->rare_points()));
                ui->army_tree->topLevelItem(4)->setExpanded(true);
                break;
            default:
                delete item;
                break;
            }
        }
        break;
    }
    }
    // unit type points column
    if (!adding) {
        int pts_col = static_cast<int>(ArmyTreeColumn::POINTS);
        switch (u->unit_type()) {
        case armies::UnitType::LORD:
            ui->army_tree->topLevelItem(0)->setText(pts_col, QString("%1").arg(army->lord_points()));
            break;
        case armies::UnitType::HERO:
            ui->army_tree->topLevelItem(1)->setText(pts_col, QString("%1").arg(army->hero_points()));
            break;
        case armies::UnitType::CORE:
            ui->army_tree->topLevelItem(2)->setText(pts_col, QString("%1").arg(army->core_points()));
            break;
        case armies::UnitType::SPECIAL:
            ui->army_tree->topLevelItem(3)->setText(pts_col, QString("%1").arg(army->special_points()));
            break;
        case armies::UnitType::RARE:
            ui->army_tree->topLevelItem(4)->setText(pts_col, QString("%1").arg(army->rare_points()));
            break;
        default: break;
        }
    }
}
