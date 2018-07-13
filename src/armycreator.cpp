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

    ui->army_tree->header()->resizeSection(0, 250); // unit name header
    ui->army_tree->header()->resizeSection(1, 80); // unit size header
    ui->army_tree->header()->resizeSection(2, 150); // weapon header
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
        lord_item->setText(0, QString(l->name.data()));
        ui->roster_tree->topLevelItem(0)->addChild(lord_item);
        ui->roster_tree->topLevelItem(0)->setExpanded(true);
    }
    for (const auto& h : heroes) {
        QTreeWidgetItem* hero_item = new QTreeWidgetItem();
        hero_item->setText(0, QString(h->name.data()));
        ui->roster_tree->topLevelItem(1)->addChild(hero_item);
        ui->roster_tree->topLevelItem(1)->setExpanded(true);
    }
    for (const auto& c : cores) {
        QTreeWidgetItem* core_item = new QTreeWidgetItem();
        core_item->setText(0, QString(c->name.data()));
        ui->roster_tree->topLevelItem(2)->addChild(core_item);
        ui->roster_tree->topLevelItem(2)->setExpanded(true);
    }
    for (const auto& s : specials) {
        QTreeWidgetItem* spec_item = new QTreeWidgetItem();
        spec_item->setText(0, QString(s->name.data()));
        ui->roster_tree->topLevelItem(3)->addChild(spec_item);
        ui->roster_tree->topLevelItem(3)->setExpanded(true);
    }
    for (const auto& r : rares) {
        QTreeWidgetItem* rare_item = new QTreeWidgetItem();
        rare_item->setText(0, QString(r->name.data()));
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

void ArmyCreator::initialise_unit_options_box() {
    int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
    auto opt_weapons = army->get_unit(curr_id).optional_weapons();

    QVBoxLayout* vbox = new QVBoxLayout;
    for (const auto& x : opt_weapons) {
        QRadioButton* b = new QRadioButton(tr(x.first.data()));
        b->setObjectName(tr((x.first + "_radiobutton").data()));
        vbox->addWidget(b);
    }
    vbox->addStretch(1);
    ui->options_group_box->setLayout(vbox);

    //QGridLayout* grid = new QGridLayout;
    //grid->addWidget(ui->options_group_box);
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
        ui->army_tree->topLevelItem(i)->setText(4, QString("%1").arg(static_cast<double>(0.0)));
    st->reset(race, *army);
    id_counter = 0;
    // now fill the roster tree widget
    clear_roster_tree();
    clear_army_tree();
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
    }
    else
        ui->add_button->setEnabled(false);
}

void ArmyCreator::on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    if (item == nullptr) return;
    std::string name = item->text(column).toStdString();
    if (name == "Lords" || name == "Heroes" || name == "Core" ||
            name == "Special" || name == "Rare") return;
    st->change_selection(name);
    on_add_button_clicked();
}

void ArmyCreator::on_army_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare") {
        ui->remove_button->setEnabled(true);
        clear_unit_options_box();
        initialise_unit_options_box();
    }
    else
        ui->remove_button->setEnabled(false);
}

void ArmyCreator::on_add_button_clicked() {
    st->add_unit_to_army_list(id_counter++);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    // now add to army tree widget
    armies::UnitType unit_type = st->selected().type();
    QTreeWidgetItem* item = new QTreeWidgetItem();
    QVariant v(st->selected().id());
    item->setData(0, Qt::UserRole, v);
    item->setText(0, QString(st->selected().name().data()));
    item->setText(1, QString("%1").arg(st->selected().size()));
    // get the current weapon of the unit
    auto weapon = st->selected().weapon();
    item->setText(2, QString("%1 [%2]").arg(weapon.first.data(), QString("%1").arg(weapon.second)));
    // get the current armour of the unit
    auto all_armour = st->selected().armour();
    auto armour = all_armour[ArmourType::ARMOUR];
    auto shield = all_armour[ArmourType::SHIELD];
    auto helmet = all_armour[ArmourType::HELMET];
    QString armour_str("");
    if (!(armour.first.empty()))
        armour_str += QString("%1 [%2").arg(armour.first.data(), QString("%1").arg(armour.second));
    if (!(shield.first.empty()))
        armour_str += QString("%1 [%2").arg(shield.first.data(), QString("%1").arg(shield.second));
    if (!(helmet.first.empty()))
        armour_str += QString("%1 [%2").arg(helmet.first.data(), QString("%1").arg(helmet.second));
    item->setText(3, armour_str);
    item->setText(4, QString("%2").arg(st->selected().points()));
    switch (unit_type) {
    case armies::UnitType::LORD:
        ui->army_tree->topLevelItem(0)->addChild(item);
        ui->army_tree->topLevelItem(0)->setText(4, QString("%1").arg(army->lord_points()));
        ui->army_tree->topLevelItem(0)->setExpanded(true);
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->addChild(item);
        ui->army_tree->topLevelItem(1)->setText(4, QString("%1").arg(army->hero_points()));
        ui->army_tree->topLevelItem(1)->setExpanded(true);
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->addChild(item);
        ui->army_tree->topLevelItem(2)->setText(4, QString("%1").arg(army->core_points()));
        ui->army_tree->topLevelItem(2)->setExpanded(true);
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->addChild(item);
        ui->army_tree->topLevelItem(3)->setText(4, QString("%1").arg(army->special_points()));
        ui->army_tree->topLevelItem(3)->setExpanded(true);
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->addChild(item);
        ui->army_tree->topLevelItem(4)->setText(4, QString("%1").arg(army->rare_points()));
        ui->army_tree->topLevelItem(4)->setExpanded(true);
        break;
    default:
        delete item;
        break;
    }
}

void ArmyCreator::on_remove_button_clicked() {
    QTreeWidgetItem* item = ui->army_tree->currentItem();
    QVariant v = item->data(0, Qt::UserRole);
    int id = v.toInt();
    armies::UnitType unit_type = army->get_unit(id).type();
    army->remove_unit(id);
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
    switch (unit_type) {
    case armies::UnitType::LORD:
        ui->army_tree->topLevelItem(0)->setText(4, QString("%1").arg(army->lord_points()));
        break;
    case armies::UnitType::HERO:
        ui->army_tree->topLevelItem(1)->setText(4, QString("%1").arg(army->hero_points()));
        break;
    case armies::UnitType::CORE:
        ui->army_tree->topLevelItem(2)->setText(4, QString("%1").arg(army->core_points()));
        break;
    case armies::UnitType::SPECIAL:
        ui->army_tree->topLevelItem(3)->setText(4, QString("%1").arg(army->special_points()));
        break;
    case armies::UnitType::RARE:
        ui->army_tree->topLevelItem(4)->setText(4, QString("%1").arg(army->rare_points()));
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
        ui->army_tree->topLevelItem(i)->setText(4, QString("%1").arg(static_cast<double>(0.0)));
    clear_army_tree();
    clear_unit_options_box();
}
