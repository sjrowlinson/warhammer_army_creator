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

    //QTreeWidgetItem* tree_item = new QTreeWidgetItem(ui->treeWidget);
    //tree_item->setText(0, QString("foo"));
    //ui->treeWidget->addTopLevelItem(tree_item);
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
        ui->treeWidget->topLevelItem(0)->addChild(lord_item);
    }
    for (const auto& h : heroes) {
        QTreeWidgetItem* hero_item = new QTreeWidgetItem();
        hero_item->setText(0, QString(h->name.data()));
        ui->treeWidget->topLevelItem(1)->addChild(hero_item);
    }
    for (const auto& c : cores) {
        QTreeWidgetItem* core_item = new QTreeWidgetItem();
        core_item->setText(0, QString(c->name.data()));
        ui->treeWidget->topLevelItem(2)->addChild(core_item);
    }
    for (const auto& s : specials) {
        QTreeWidgetItem* spec_item = new QTreeWidgetItem();
        spec_item->setText(0, QString(s->name.data()));
        ui->treeWidget->topLevelItem(3)->addChild(spec_item);
    }
    for (const auto& r : rares) {
        QTreeWidgetItem* rare_item = new QTreeWidgetItem();
        rare_item->setText(0, QString(r->name.data()));
        ui->treeWidget->topLevelItem(4)->addChild(rare_item);
    }
}

void ArmyCreator::clear_roster_tree() {
    for (int i = 0; i < 5; ++i)
        ui->treeWidget->topLevelItem(i)->takeChildren();
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
    st->reset(race, *army);
    // now fill the roster tree widget
    clear_roster_tree();
    populate_roster_tree();
}

void ArmyCreator::on_pts_limit_spinbox_valueChanged(double pts) {
    army->change_points_limit(pts);
}

void ArmyCreator::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    std::string name = current->text(0).toStdString();
    if (name != "Lords" && name != "Heroes" && name != "Core" &&
            name != "Special" && name != "Rare")
        st->change_selection(name);
}

void ArmyCreator::on_add_button_clicked() {
    st->add_unit_to_army_list();
    ui->current_pts_label->setText(QString("%1").arg(army->current_points()));
}
