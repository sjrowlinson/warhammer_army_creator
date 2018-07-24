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
    ui->army_tree->header()->resizeSection(1, 80); // unit size header
    ui->army_tree->header()->resizeSection(2, 150); // melee weapon header
    ui->army_tree->header()->resizeSection(3, 150); // ranged weapon header
    ui->army_tree->header()->resizeSection(4, 150); // armour header
    ui->army_tree->header()->resizeSection(5, 80); // points header
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

void ArmyCreator::initialise_unit_options_box(bool from_roster) {
    std::unordered_map<
        std::string,
        std::tuple<WeaponType, ItemClass, double>
    > opt_weapons;
    if (from_roster) {
        switch (st->selected()->base_unit_type()) {
        case BaseUnitType::MELEE_CHARACTER:
            opt_weapons = std::dynamic_pointer_cast<melee_character_unit>(
                            st->selected()
                          )->handle->opt().opt_weapons;
            break;
        case BaseUnitType::MAGE_CHARACTER:
            opt_weapons = std::dynamic_pointer_cast<mage_character_unit>(
                            st->selected()
                          )->handle->opt().opt_weapons;
            break;
        case BaseUnitType::MIXED:
            // TODO: implement once mixed units are figured out
            break;
        case BaseUnitType::NORMAL:
            opt_weapons = std::dynamic_pointer_cast<normal_unit>(
                            st->selected()
                          )->handle->opt().opt_weapons;
            break;
        default: break;
        }
    }
    else {
        int curr_id = ui->army_tree->currentItem()->data(0, Qt::UserRole).toInt();
        switch (army->get_unit(curr_id)->base_unit_type()) {
        case BaseUnitType::MELEE_CHARACTER:
            opt_weapons = std::dynamic_pointer_cast<melee_character_unit>(army->get_unit(curr_id))->handle->opt().opt_weapons;
            break;
        case BaseUnitType::MAGE_CHARACTER:
            opt_weapons = std::dynamic_pointer_cast<mage_character_unit>(army->get_unit(curr_id))->handle->opt().opt_weapons;
            break;
        case BaseUnitType::MIXED:
            // TODO: implement once mixed units are figured out
            break;
        case BaseUnitType::NORMAL:
            opt_weapons = std::dynamic_pointer_cast<normal_unit>(army->get_unit(curr_id))->handle->opt().opt_weapons;
            break;
        default: break;
        }
    }
    QVBoxLayout* vbox = new QVBoxLayout;
    for (const auto& x : opt_weapons) {
        QRadioButton* b = new QRadioButton(tr(std::get<0>(x).data()));
        b->setObjectName(tr((std::get<0>(x) + "_radiobutton").data()));
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
    st->change_selection(name);
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
        if (!(std::get<1>(mw).empty()))
            item->setText(2, QString("%1 [%2]").arg(
                std::get<1>(mw).data(), 
                QString("%1").arg(std::get<2>(mw))
                )
            );
        if (!(std::get<1>(rw).empty()))
            item->setText(3, QString("%1 [%2]").arg(
                std::get<1>(rw).data(), 
                QString("%1").arg(std::get<2>(rw))
                )
            );
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
        item->setText(4, armour_str);
        item->setText(5, QString("%2").arg(p->points()));
        // unit type
        auto unit_type = p->unit_type();
        switch (unit_type) {
        case armies::UnitType::LORD:
            ui->army_tree->topLevelItem(0)->addChild(item);
            ui->army_tree->topLevelItem(0)->setText(5, QString("%1").arg(army->lord_points()));
            ui->army_tree->topLevelItem(0)->setExpanded(true);
            break;
        case armies::UnitType::HERO:
            ui->army_tree->topLevelItem(1)->addChild(item);
            ui->army_tree->topLevelItem(1)->setText(5, QString("%1").arg(army->hero_points()));
            ui->army_tree->topLevelItem(1)->setExpanded(true);
            break;
        case armies::UnitType::CORE:
            ui->army_tree->topLevelItem(2)->addChild(item);
            ui->army_tree->topLevelItem(2)->setText(5, QString("%1").arg(army->core_points()));
            ui->army_tree->topLevelItem(2)->setExpanded(true);
            break;
        case armies::UnitType::SPECIAL:
            ui->army_tree->topLevelItem(3)->addChild(item);
            ui->army_tree->topLevelItem(3)->setText(5, QString("%1").arg(army->special_points()));
            ui->army_tree->topLevelItem(3)->setExpanded(true);
            break;
        case armies::UnitType::RARE:
            ui->army_tree->topLevelItem(4)->addChild(item);
            ui->army_tree->topLevelItem(4)->setText(5, QString("%1").arg(army->rare_points()));
            ui->army_tree->topLevelItem(4)->setExpanded(true);
            break;
        default:
            delete item;
            break;
        }
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
        if (!(std::get<1>(mw).empty()))
            item->setText(2, QString("%1 [%2]").arg(
                std::get<1>(mw).data(),
                QString("%1").arg(std::get<2>(mw))
                )
            );
        if (!(std::get<1>(rw).empty()))
            item->setText(3, QString("%1 [%2]").arg(
                std::get<1>(rw).data(),
                QString("%1").arg(std::get<2>(rw))
                )
            );
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
        item->setText(4, armour_str);
        item->setText(5, QString("%2").arg(p->points()));
        // unit type
        auto unit_type = p->unit_type();
        switch (unit_type) {
        case armies::UnitType::LORD:
            ui->army_tree->topLevelItem(0)->addChild(item);
            ui->army_tree->topLevelItem(0)->setText(5, QString("%1").arg(army->lord_points()));
            ui->army_tree->topLevelItem(0)->setExpanded(true);
            break;
        case armies::UnitType::HERO:
            ui->army_tree->topLevelItem(1)->addChild(item);
            ui->army_tree->topLevelItem(1)->setText(5, QString("%1").arg(army->hero_points()));
            ui->army_tree->topLevelItem(1)->setExpanded(true);
            break;
        case armies::UnitType::CORE:
            ui->army_tree->topLevelItem(2)->addChild(item);
            ui->army_tree->topLevelItem(2)->setText(5, QString("%1").arg(army->core_points()));
            ui->army_tree->topLevelItem(2)->setExpanded(true);
            break;
        case armies::UnitType::SPECIAL:
            ui->army_tree->topLevelItem(3)->addChild(item);
            ui->army_tree->topLevelItem(3)->setText(5, QString("%1").arg(army->special_points()));
            ui->army_tree->topLevelItem(3)->setExpanded(true);
            break;
        case armies::UnitType::RARE:
            ui->army_tree->topLevelItem(4)->addChild(item);
            ui->army_tree->topLevelItem(4)->setText(5, QString("%1").arg(army->rare_points()));
            ui->army_tree->topLevelItem(4)->setExpanded(true);
            break;
        default:
            delete item;
            break;
        }
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
        if (!(std::get<1>(mw).empty()))
            item->setText(2, QString("%1 [%2]").arg(
                std::get<1>(mw).data(),
                QString("%1").arg(std::get<2>(mw))
                )
            );
        if (!(std::get<1>(rw).empty()))
            item->setText(3, QString("%1 [%2]").arg(
                std::get<1>(rw).data(),
                QString("%1").arg(std::get<2>(rw))
                )
            );
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
        item->setText(4, armour_str);
        item->setText(5, QString("%2").arg(p->points()));
        // unit type
        auto unit_type = p->unit_type();
        switch (unit_type) {
        case armies::UnitType::LORD:
            ui->army_tree->topLevelItem(0)->addChild(item);
            ui->army_tree->topLevelItem(0)->setText(5, QString("%1").arg(army->lord_points()));
            ui->army_tree->topLevelItem(0)->setExpanded(true);
            break;
        case armies::UnitType::HERO:
            ui->army_tree->topLevelItem(1)->addChild(item);
            ui->army_tree->topLevelItem(1)->setText(5, QString("%1").arg(army->hero_points()));
            ui->army_tree->topLevelItem(1)->setExpanded(true);
            break;
        case armies::UnitType::CORE:
            ui->army_tree->topLevelItem(2)->addChild(item);
            ui->army_tree->topLevelItem(2)->setText(5, QString("%1").arg(army->core_points()));
            ui->army_tree->topLevelItem(2)->setExpanded(true);
            break;
        case armies::UnitType::SPECIAL:
            ui->army_tree->topLevelItem(3)->addChild(item);
            ui->army_tree->topLevelItem(3)->setText(5, QString("%1").arg(army->special_points()));
            ui->army_tree->topLevelItem(3)->setExpanded(true);
            break;
        case armies::UnitType::RARE:
            ui->army_tree->topLevelItem(4)->addChild(item);
            ui->army_tree->topLevelItem(4)->setText(5, QString("%1").arg(army->rare_points()));
            ui->army_tree->topLevelItem(4)->setExpanded(true);
            break;
        default:
            delete item;
            break;
        }
        break;
    }
    default: break;
    }
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
