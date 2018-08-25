#include "armycreator.h"
#include "optionbox.h"
#include "magicitembox.h"
#include "ui_armycreator.h"

ArmyCreator::ArmyCreator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmyCreator) {
    // core GUI element initialisation and resizing
    ui->setupUi(this);
    setFixedSize(1800, 1050);
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::NAME), 250); // unit name header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::SIZE), 60); // unit size header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::WEAPONS), 180); // weapons header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::ARMOUR), 150); // armour header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::COMMAND), 180); // command group header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::EXTRAS), 180); // extras header
    ui->army_tree->header()->resizeSection(static_cast<int>(ArmyTreeColumn::POINTS), 60); // points header
    ui->duplicate_button->setEnabled(false);
    ui->remove_button->setEnabled(false);
    // core logic initialisation
    race = armies::s_map_string_faction[
        ui->faction_combobox->currentText().toStdString()
    ];
    army = std::make_shared<army_list>(ui->pts_limit_spinbox->value());
    st = std::make_shared<selection_tree>(race, *army);
    current = nullptr;
    opt_sel = std::make_shared<option_selector>(st, army);
    ob = std::make_shared<OptionBox>(this, ui->options_group_box);
    mib = std::make_shared<MagicItemBox>(this, ui->magic_items_selector);
    populate_roster_tree();
    id_counter = 0; // unit ID counter
    in_tree = InTree::NEITHER;
    // initialise validity reasons label
    update_validity_label();
    ui->validity_reasons_label->setWordWrap(true);
    // initialise magic items combobox
    ui->magic_items_combobox->addItem(QString("Common"), QVariant(3));
    if (!st->magic_items_name().empty())
        ui->magic_items_combobox->addItem(QString(st->magic_items_name().data()), QVariant(1));
    if (!st->faction_items_name().empty())
        ui->magic_items_combobox->addItem(QString(st->faction_items_name().data()), QVariant(2));
}

ArmyCreator::~ArmyCreator() {
    delete ui;
}

void ArmyCreator::on_actionExit_triggered() {
    QCoreApplication::quit();
}

void ArmyCreator::update_validity_label() {
    auto reasons = army->invalid_reasons();
    if (reasons.empty()) { // valid list
        ui->validity_reasons_label->setText(tr(""));
        ui->validity_label->setStyleSheet("QLabel { font-size : 10px; } ");
        QPalette palette = ui->validity_label->palette();
        palette.setColor(ui->validity_label->foregroundRole(), Qt::darkGreen);
        ui->validity_label->setPalette(palette);
        ui->validity_label->setText("Valid List");
        return;
    }
    QPalette pt = ui->validity_label->palette();
    pt.setColor(ui->validity_label->foregroundRole(), Qt::red);
    ui->validity_label->setPalette(pt);
    ui->validity_label->setText("Invalid List:");
    std::string vrl_text = "";
    double pts_limit = ui->pts_limit_spinbox->value();
    for (const auto& r : reasons) {
        switch (r) {
        case InvalidListReason::POINTS:
            vrl_text += "Too many points [" +
                     tools::points_str(army->current_points()) + "/" +
                     tools::points_str(pts_limit) + "]";
            break;
        case InvalidListReason::LORD_LIMIT:
            if (!vrl_text.empty()) vrl_text += ", ";
            vrl_text += "Too many Lord Unit points [" +
                     tools::points_str(army->lord_points()) + "/" +
                     tools::points_str(0.25*pts_limit) + "]";
            break;
        case InvalidListReason::HERO_LIMIT:
            if (!vrl_text.empty()) vrl_text += ", ";
            vrl_text += "Too many Hero Unit points [" +
                     tools::points_str(army->hero_points()) + "/" +
                     tools::points_str(0.25*pts_limit) + "]";
            break;
        case InvalidListReason::CORE_MINIMUM:
            if (!vrl_text.empty()) vrl_text += ", ";
            vrl_text += "Not enough Core Unit points [" +
                     tools::points_str(army->core_points()) + "/" +
                     tools::points_str(0.25*pts_limit) + "]";
            break;
        case InvalidListReason::SPECIAL_LIMIT:
            if (!vrl_text.empty()) vrl_text += ", ";
            vrl_text += "Too many Special Unit points [" +
                     tools::points_str(army->special_points()) + "/" +
                     tools::points_str(0.5*pts_limit) + "]";
            break;
        case InvalidListReason::RARE_LIMIT:
            if (!vrl_text.empty()) vrl_text += ", ";
            vrl_text += "Too many Rare Unit points [" +
                     tools::points_str(army->rare_points()) + "/" +
                     tools::points_str(0.25*pts_limit) + "]";
            break;
        }
    }
    ui->validity_reasons_label->setText(tr(vrl_text.data()));
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

// option box selection slots

void ArmyCreator::change_unit_size() {
    QSpinBox* sb = qobject_cast<QSpinBox*>(QObject::sender());
    std::shared_ptr<unit> curr;
    int curr_id = -1;
    switch (in_tree) {
    case InTree::ARMY:
        curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        curr = army->get_unit(curr_id);
        break;
    case InTree::ROSTER:
        curr = st->selected();
        break;
    default: throw std::runtime_error("No unit selected!");
    }
    switch (curr->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(curr);
        try {
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(curr_id);
                p->change_size(static_cast<std::size_t>(sb->value()));
                army->update_on(curr_id);
                ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::SIZE);
                update_validity_label();
                break;
            case InTree::ROSTER:
                p->change_size(static_cast<std::size_t>(sb->value()));
                break;
            default: throw std::runtime_error("No unit selected!");
            }
        } catch (const std::invalid_argument&) {}
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(curr);
        try {
            if (tools::split(sb->objectName().toStdString(), '_')[1] == "master")
                p->switch_mixed_select(MixedSelect::MASTER);
            switch (in_tree) {
            case InTree::ARMY:
                army->take_snapshot_of(curr_id);
                p->change_size(static_cast<std::size_t>(sb->value()));
                army->update_on(curr_id);
                ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
                update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::SIZE);
                update_validity_label();
                break;
            case InTree::ROSTER:
                p->change_size(static_cast<std::size_t>(sb->value()));
                break;
            default: throw std::runtime_error("No unit selected!");
            }
            p->switch_mixed_select(MixedSelect::SLAVE);
        } catch (const std::invalid_argument& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, tr("Error"), tr(e.what()));
            message_box.setFixedSize(500, 200);
            if (tools::split(sb->objectName().toStdString(), '_')[1] == "master")
                sb->setValue(sb->value() - 1);
            else sb->setValue(sb->value() + 1);
        }
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
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::ARMOUR);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    // TODO: clear and re-initialise just the weapons section of unit options box
    ob->clear();
    ob->reset(current, in_tree);
    ob->reinitialise();
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::WEAPON);
}

void ArmyCreator::optional_armour_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_armour(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::ARMOUR);
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::WEAPONS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    ob->clear();
    ob->reset(current, in_tree);
    ob->reinitialise();
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::ARMOUR);
}

void ArmyCreator::optional_talisman_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_talisman(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::TALISMAN);
}

void ArmyCreator::optional_enchanted_item_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_enchanted_item(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::ENCHANTED);
}

void ArmyCreator::optional_other_item_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    try {
        if (opt_sel->select_other_item(cb_name, cb->isChecked())) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::OTHER);
}

void ArmyCreator::optional_banner_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_banner(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::BANNER);
}

void ArmyCreator::optional_arcane_item_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_arcane_item(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::ARCANE);
}

void ArmyCreator::optional_level_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_mage_level(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::NAME);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
    }
}

void ArmyCreator::optional_mount_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_mount(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::NAME);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
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
                ob->clear();
                ob->reset(current, in_tree);
                ob->reinitialise();
            }
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
    }
}

void ArmyCreator::optional_oco_extra_selected() {
    QRadioButton* rb = qobject_cast<QRadioButton*>(QObject::sender());
    std::string rb_name = rb->objectName().toStdString();
    try {
        if (opt_sel->select_oco_extra(rb_name)) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
    }
}

void ArmyCreator::optional_mc_extra_selected() {
    QCheckBox* cb = qobject_cast<QCheckBox*>(QObject::sender());
    std::string cb_name = cb->objectName().toStdString();
    try {
        if (opt_sel->select_mc_extra(cb_name, cb->isChecked())) {
            ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(ui->army_tree->currentItem(), false, ArmyTreeColumn::EXTRAS);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
    }
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
    ui->unit_info_box->setTitle(tr(u->name().data()));
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
        QStringList header = {"M", "WS", "BS", "S", "T", "W", "I", "A", "LD"};
        for (auto i = 0; i < header.size(); ++i) {
            QTableWidgetItem* entry = new QTableWidgetItem(header[i]);
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
                QTableWidgetItem* entry = new QTableWidgetItem(tr(x.data()));
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
                QTableWidgetItem* entry = new QTableWidgetItem(tr(x.data()));
                entry->setFlags(entry->flags() & ~Qt::ItemIsEditable);
                stats_table->setItem(1, count++, entry);
            }
            auto champ_stats = p->handle->champion_statistics();
            if (!champ_stats.empty() &&
                    !std::equal(
                        std::begin(stats),
                        std::end(stats),
                        std::begin(champ_stats)
                     )) {
                stats_table->insertRow(2);
                count = 0;
                for (const auto& x : champ_stats) {
                    QTableWidgetItem* entry = new QTableWidgetItem(tr(x.data()));
                    entry->setFlags(entry->flags() & ~Qt::ItemIsEditable);
                    stats_table->setItem(2, count++, entry);
                }
            }
            break;
        }
        default: return;
        }
        stats_table->resizeRowsToContents();
        stats_table->resizeColumnsToContents();
        vbox->addWidget(stats_table);
    }
    // special rules
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
        special_rules_label->setWordWrap(true);
        vbox->addWidget(special_rules_label);
    }
    ui->unit_info_box->setLayout(vbox);
}

void ArmyCreator::on_faction_combobox_currentTextChanged(const QString& faction) {
    race = armies::s_map_string_faction[faction.toStdString()];
    army->clear();
    ui->current_pts_label->setText(QString("%1").arg(static_cast<double>(0.0)));
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->setText(6, QString("%1").arg(static_cast<double>(0.0)));
    st->reset(race, *army);
    current = nullptr;
    id_counter = 0;
    clear_roster_tree();
    clear_army_tree();
    clear_unit_info_box();
    ob->clear();
    mib->clear();
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
}

void ArmyCreator::on_magic_items_combobox_currentTextChanged(const QString& ic_select) {
    (void)(ic_select);
    auto v = ui->magic_items_combobox->currentData().toInt();
    mib->clear();
    mib->reset(current, in_tree);
    mib->reset_category(static_cast<ItemClass>(v));
    mib->reinitialise(ItemType::WEAPON);
}

void ArmyCreator::on_pts_limit_spinbox_valueChanged(double pts) {
    army->change_points_limit(pts);
    update_validity_label();
}

// tree item changed or activated slots

void ArmyCreator::on_roster_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    ui->army_tree->setCurrentItem(ui->army_tree->topLevelItem(0));
    ui->army_tree->clearSelection();
    ui->roster_tree->setCurrentItem(current);
    clear_unit_info_box();
    ob->clear();
    mib->clear();
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        st->change_selection(name);
        this->current = st->selected();
        in_tree = InTree::ROSTER;
        initialise_unit_info_box();
        opt_sel->reset(this->current, in_tree);
        ob->reset(this->current, in_tree);
        ob->reinitialise();
        mib->reset(this->current, in_tree);
        mib->reinitialise(ItemType::WEAPON);
        ui->add_button->setEnabled(true);
    }
    else {
        this->current = nullptr;
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
    ob->clear();
    mib->clear();
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        in_tree = InTree::ARMY;
        int id = current->data(0, Qt::UserRole).toInt();
        this->current = army->get_unit(id);
        initialise_unit_info_box();
        opt_sel->reset(this->current, in_tree);
        ob->reset(this->current, in_tree);
        ob->reinitialise();
        mib->reset(this->current, in_tree);
        mib->reinitialise(ItemType::WEAPON);
        ui->remove_button->setEnabled(true);
        ui->duplicate_button->setEnabled(true);
    }
    else {
        this->current = nullptr;
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
    current = st->selected();
    clear_unit_info_box();
    ob->clear();
    mib->clear();
    // just make sure to keep us in roster tree
    // in case it gets changed somehow
    in_tree = InTree::ROSTER;
    initialise_unit_info_box();
    opt_sel->reset(current, in_tree);
    ob->reset(current, in_tree);
    ob->reinitialise();
    mib->reset(current, in_tree);
    mib->reinitialise(ItemType::WEAPON);
    update_validity_label();
}

void ArmyCreator::on_duplicate_button_clicked() {
    QTreeWidgetItem* curr_item = ui->army_tree->currentItem();
    auto u = army->get_unit(curr_item->data(0, Qt::UserRole).toInt());
    // TODO: add check to unit class as to whether it has any magic items and
    // prevent the duplication if so
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
    ob->clear();
    ob->reinitialise();
    update_validity_label();
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
    ob->clear();
    mib->clear();
    update_validity_label();
    delete item;
}

void ArmyCreator::on_clear_button_clicked() {
    army->clear();
    ui->current_pts_label->setText(QString("%1").arg(static_cast<double>(0.0)));
    for (int i = 0; i < 5; ++i)
        ui->army_tree->topLevelItem(i)->setText(6, QString("%1").arg(static_cast<double>(0.0)));
    clear_army_tree();
    ob->clear();
    mib->clear();
    update_validity_label();
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
        std::string mlevel_str = "";
        if (u->is_mage()) {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(u);
            mlevel_str = " [Level " + std::to_string(p->level()) + "] ";
        }
        std::string name = u->name() + mlevel_str + ((u->mount().first.empty()) ? "" : "\n(" + u->mount().first + ")");
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
            QString command_str;
            update_unit_command_display_helper(p->command(), command_str);
            item->setText(static_cast<int>(column), command_str);
            break;
        }
        case BaseUnitType::MIXED:
        {
            auto p = std::dynamic_pointer_cast<mixed_unit>(u);
            QString command_str;
            update_unit_command_display_helper(p->master().command(), command_str);
            update_unit_command_display_helper(p->slave().command(), command_str);
            item->setText(static_cast<int>(column), command_str);
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
        if (!mc_extras.empty()) {
            for (auto it = std::begin(mc_extras),
                 it_next = std::next(std::begin(mc_extras)); it != std::end(mc_extras); ++it) {

                extras_str += QString("%1 [%2]").arg(
                                          it->first.data(),
                                          QString("%1").arg(it->second.second)
                                      );
                if (it_next != std::end(mc_extras)) {
                    extras_str += QString("\n");
                    ++it_next;
                }
            }
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
            if (!champ_mc_extras.empty()) {
                for (auto it = std::begin(champ_mc_extras),
                     it_next = std::next(std::begin(champ_mc_extras)); it != std::end(champ_mc_extras); ++it) {
                    extras_str += QString("%1 [%2]").arg(
                                              (it->first + " (Champion)").data(),
                                              QString("%1").arg(it->second.second)
                                          );
                    if (it_next != std::end(champ_mc_extras)) {
                        extras_str += QString("\n");
                        ++it_next;
                    }
                }
            }

        }
        u->switch_model_select(ModelSelect::DEFAULT);
        if (u->is_character()) {
            auto p = std::dynamic_pointer_cast<character_unit>(u);
            if (!p->talisman().first.empty()) {
                if (!extras_str.isEmpty()) extras_str += QString("\n");
                extras_str += QString("%1 [%2]").arg(
                    p->talisman().first.data(),
                    QString("%1").arg(p->talisman().second.second)
                );
            }
            if (!p->enchanted_item().first.empty()) {
                if (!extras_str.isEmpty()) extras_str += QString("\n");
                extras_str += QString("%1 [%2]").arg(
                    p->enchanted_item().first.data(),
                    QString("%1").arg(p->enchanted_item().second.second)
                );
            }
            if (!p->item_extras().empty()) {
                if (!extras_str.isEmpty()) extras_str += QString("\n");
                auto count = 0;
                for (const auto& e : p->item_extras()) {
                    if (count) extras_str += QString("\n");
                    extras_str += QString("%1 [%2]").arg(
                        e.first.data(),
                        QString("%1").arg(e.second.second)
                    );
                    ++count;
                }
            }
            if (u->is_mage()) {
                auto p_ = std::dynamic_pointer_cast<mage_character_unit>(u);
                if (!p_->arcane_item().first.empty()) {
                    if (!extras_str.isEmpty()) extras_str += QString("\n");
                    extras_str += QString("%1 [%2]").arg(
                        p_->arcane_item().first.data(),
                        QString("%1").arg(p_->arcane_item().second.second)
                    );
                }
            }
        }
        auto banner = u->magic_banner();
        if (!(banner.first.empty())) {
            if (!extras_str.isEmpty()) extras_str += QString("\n");
            extras_str += QString("%1 [%2]").arg(
                              banner.first.data(),
                              QString("%1").arg(banner.second.second)
                          );
        }
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

void ArmyCreator::update_unit_command_display_helper(
    const std::unordered_map<CommandGroup, std::pair<std::string, double>>& command,
    QString& command_str
        ) {
    auto musician_it = command.find(CommandGroup::MUSICIAN);
    auto sb_it = command.find(CommandGroup::STANDARD_BEARER);
    auto champion_it = command.find(CommandGroup::CHAMPION);
    if (musician_it != command.end()) {
        command_str += QString("%1 [%2]").arg(
                          std::get<0>(musician_it->second).data(),
                          QString("%1").arg(std::get<1>(musician_it->second))
                       );
    }
    if (sb_it != command.end()) {
        if (!command_str.isEmpty()) command_str += QString("\n");
        command_str += QString("%1 [%2]").arg(
                          std::get<0>(sb_it->second).data(),
                          QString("%1").arg(std::get<1>(sb_it->second))
                       );
    }
    if (champion_it != command.end()) {
        if (!command_str.isEmpty()) command_str += QString("\n");
        command_str += QString("%1 [%2]").arg(
                          std::get<0>(champion_it->second).data(),
                          QString("%1").arg(std::get<1>(champion_it->second))
                       );
    }
}

void ArmyCreator::on_export_button_clicked() {
    QString str_stream;
    QTextStream out(&str_stream);
    const int row_count = ui->army_tree->model()->rowCount();
    const int col_count = ui->army_tree->model()->columnCount();
    out <<  "<html>\n"
        "<head>\n"
        "<meta Content=\"Text/html; charset=Windows-1251\">\n"
        <<  QString("<title>%1</title>\n").arg("Title")
        <<  "</head>\n"
        "<body bgcolor=#ffffff link=#5000A0>\n"
        "<table border=1 cellspacing=0 cellpadding=2>\n";
    // headers
    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < col_count; column++)
        if (!ui->army_tree->isColumnHidden(column))
            out << QString("<th>%1</th>").arg(ui->army_tree->model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";
    // data table
    for (int row = 0; row < row_count; row++) {
        out << "<tr>";
        for (int column = 0; column < col_count; column++) {
            if (!ui->army_tree->isColumnHidden(column)) {
                QString data =
                    ui->army_tree->model()->data(ui->army_tree->model()->index(row, column)).toString().simplified();
                std::cout << data.toStdString() << std::endl;
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out <<  "</table>\n"
        "</body>\n"
        "</html>\n";
    QTextDocument* document = new QTextDocument();
    document->setHtml(str_stream);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(QString("test.pdf"));
    document->setPageSize(printer.pageRect().size());
    document->print(&printer);
    delete document;
}
