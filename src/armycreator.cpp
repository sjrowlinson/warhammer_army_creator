#include "armycreator.h"
#include "optionbox.h"
#include "option_selector.h"
#include "magicitembox.h"
#include "ui_armycreator.h"

ArmyCreator::ArmyCreator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmyCreator) {
    // core GUI element initialisation and resizing
    ui->setupUi(this);
    do_splitter_scalings();
    initialise_stylesheets();
    resize_army_trees();
    ui->duplicate_button->setEnabled(false);
    ui->remove_button->setEnabled(false);
    ui->set_general_button->setEnabled(false);
    // core logic initialisation
    race = enum_convert::STRING_TO_FACTION.at(
        ui->faction_combobox->currentText().toStdString()
    );
    army = std::make_shared<army_list>(ui->points_limit_spinbox->value(), race);
    try { st = std::make_shared<selection_tree>(race, army); }
    catch (const std::runtime_error&) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr("Fatal error - unable to parse army data. Forcing exit..."));
        message_box.setFixedSize(500, 200);
        QCoreApplication::quit();
    }
    current = nullptr;
    populate_roster_tree();
    id_counter = 0; // unit ID counter
    in_tree = InTree::NEITHER;
    // initialise validity reasons label
    update_validity_label();
    // initialise magic items combobox
    setup_magic_items_combobox();
    ui->unit_options_scrollarea->setWidgetResizable(true);
    ui->magic_items_scrollarea->setWidgetResizable(true);
    // TODO: add option in a preferences/settings page for setting up the convenient directories
    //setup_export_directories();
}

std::shared_ptr<ArmyCreator> ArmyCreator::create(QWidget* parent) {
    class make_shared_enabler : public ArmyCreator {
    public:
        make_shared_enabler(QWidget* parent) : ArmyCreator(parent) {}
    };
    auto p = std::make_shared<make_shared_enabler>(parent);
    p->opt_sel = std::make_shared<option_selector>(p->shared_from_this(), p->army);
    p->ob = std::make_shared<OptionBox>(p->shared_from_this(), p->ui->options_group_box);
    p->mib = std::make_shared<MagicItemBox>(p->shared_from_this(), p->ui->magic_items_selector,
                                            p->ui->item_description_groupbox, p->ui->item_description_label);
    return std::move(p);
}

ArmyCreator::~ArmyCreator() {
    delete ui;
}

void ArmyCreator::do_splitter_scalings() {
    const int height_div = static_cast<int>(static_cast<double>(height())/40);
    const int width_div = static_cast<int>(static_cast<double>(width())/40);
    const int val_frame_width_div = static_cast<int>(static_cast<double>(ui->validity_frame->width())/5);
    ui->main_vertical_splitter->setSizes(
        QList<int>() << height_div
                     << 29*height_div
                     << 12*height_div
    );
    ui->top_horizontal_splitter->setSizes(
        QList<int>() << 6*width_div
                     << 26*width_div
                     << 8*width_div
    );
    ui->middle_horizontal_splitter->setSizes(
        QList<int>() << 6*width_div
                     << 26*width_div
                     << 8*width_div
    );
    ui->lower_horizontal_splitter->setSizes(
        QList<int>() << 32*width_div
                     << 8*width_div
    );
    ui->magic_item_budgets_splitter->setSizes(
        QList<int>() << 6*width_div
                     << 6*width_div
                     << 20*width_div
    );
    ui->magic_item_frame_vsplitter->setSizes(
        QList<int>() << height_div
                     << 11*height_div
    );
    ui->info_options_frame_vsplitter->setSizes(
        QList<int>() << 9*height_div
                     << 20*height_div
    );
    ui->army_trees_splitter->setSizes(
        QList<int>() << 4*height_div
                     << 5*height_div
                     << 9*height_div
                     << 6*height_div
                     << 5*height_div
    );
    ui->validity_labels_splitter->setSizes(
        QList<int>() << val_frame_width_div
                     << 4*val_frame_width_div
    );
}

void ArmyCreator::on_actionExit_triggered() {
    QCoreApplication::quit();
}

void ArmyCreator::initialise_stylesheets() {
    const QString tree_header_ss = "QHeaderView::section { background-color: #d2a19b }";
    auto trees = std::vector<QTreeWidget*>{ui->roster_tree, ui->lord_tree, ui->hero_tree,
                                           ui->core_tree, ui->special_tree, ui->rare_tree};
    for (auto tree : trees) tree->header()->setStyleSheet(tree_header_ss);
    //const QString tree_body_ss = "background-color: #edd9d7";
    //for (auto tree : trees) tree->setStyleSheet(tree_body_ss);
}

void ArmyCreator::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    resize_army_trees();
    do_splitter_scalings();
}

void ArmyCreator::resize_army_trees() {
    const int atf_width = ui->lord_tree->width();
    const int char_tree_cols = ui->lord_tree->columnCount();
    const int unit_tree_cols = ui->core_tree->columnCount();
    const int atf_width_div_c = static_cast<int>(static_cast<double>(atf_width)/char_tree_cols);
    const int atf_width_div_u = static_cast<int>(static_cast<double>(atf_width)/unit_tree_cols);
    for (auto tree : std::vector<QTreeWidget*>{ui->lord_tree, ui->hero_tree}) {
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::NAME), static_cast<int>(1.5*atf_width_div_c));
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::MOUNT), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::LEVEL), static_cast<int>(0.6*atf_width_div_c));
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::LORE), static_cast<int>(0.7*atf_width_div_c));
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::WEAPONS), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::ARMOUR), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::TALISMAN), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::ENCHANTED), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::ARCANE), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::BANNER), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::EXTRAS), atf_width_div_c);
        tree->header()->resizeSection(static_cast<int>(CharacterTreeColumn::POINTS), static_cast<int>(atf_width_div_c));
    }
    for (auto tree : std::vector<QTreeWidget*>{ui->core_tree, ui->special_tree, ui->rare_tree}) {
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::NAME), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::SIZE), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::MOUNT), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::WEAPONS), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::ARMOUR), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::EXTRAS), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::COMMAND), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::BANNER), atf_width_div_u);
        tree->header()->resizeSection(static_cast<int>(UnitTreeColumn::POINTS), atf_width_div_u);
    }
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
    double pts_limit = ui->points_limit_spinbox->value();
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

void ArmyCreator::update_budget_label() {
    if (current->is_character()) {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        std::string budget_str = "Magic: " +
                tools::points_str(p->handle_->magic_item_budget().points - p->magic_item_points());
        if (p->handle_->faction_items_handle() != nullptr) {
            budget_str += "  " + p->handle_->faction_items_handle()->first + ": " +
                tools::points_str(p->handle_->faction_item_budget().points - p->faction_item_points()) +
                "   Total: " + tools::points_str(p->handle_->total_item_budget().points - p->total_item_points());
        }
        ui->budget_remaining_label->setText(QString::fromStdString(budget_str));
    } else {
        if (current->is_mixed()) {
            // TODO: implement
        } else {
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
            std::string budget_str = "Magic: " +
                    tools::points_str(p->handle->magic_item_budget().points - p->magic_item_points());
            if (p->handle->faction_items_handle() != nullptr) {
                budget_str += "  " + p->handle->faction_items_handle()->first + ": " +
                    tools::points_str(p->handle->faction_item_budget().points - p->faction_item_points()) +
                    "   Total: " + tools::points_str(p->handle->total_item_budget().points - p->total_item_points());
            }
            budget_str += "  Banner: " + tools::points_str(p->handle->magic_banner_budget() -
                                                           p->magic_banner().second.second);
            ui->budget_remaining_label->setText(QString::fromStdString(budget_str));
        }

    }
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
        lord_item->setText(static_cast<int>(ArmyTreeColumn::NAME), QString(l->name().data()));
        ui->roster_tree->topLevelItem(0)->addChild(lord_item);
        ui->roster_tree->topLevelItem(0)->setExpanded(true);
    }
    for (const auto& h : heroes) {
        QTreeWidgetItem* hero_item = new QTreeWidgetItem();
        hero_item->setText(static_cast<int>(ArmyTreeColumn::NAME), QString(h->name().data()));
        ui->roster_tree->topLevelItem(1)->addChild(hero_item);
        ui->roster_tree->topLevelItem(1)->setExpanded(true);
    }
    for (const auto& c : cores) {
        QTreeWidgetItem* core_item = new QTreeWidgetItem();
        core_item->setText(static_cast<int>(ArmyTreeColumn::NAME), QString(c->name().data()));
        ui->roster_tree->topLevelItem(2)->addChild(core_item);
        ui->roster_tree->topLevelItem(2)->setExpanded(true);
    }
    for (const auto& s : specials) {
        QTreeWidgetItem* spec_item = new QTreeWidgetItem();
        spec_item->setText(static_cast<int>(ArmyTreeColumn::NAME), QString(s->name().data()));
        ui->roster_tree->topLevelItem(3)->addChild(spec_item);
        ui->roster_tree->topLevelItem(3)->setExpanded(true);
    }
    for (const auto& r : rares) {
        QTreeWidgetItem* rare_item = new QTreeWidgetItem();
        rare_item->setText(static_cast<int>(ArmyTreeColumn::NAME), QString(r->name().data()));
        ui->roster_tree->topLevelItem(4)->addChild(rare_item);
        ui->roster_tree->topLevelItem(4)->setExpanded(true);
    }
}

// ui element clearers

void ArmyCreator::clear_roster_tree() {
    for (int i = 0; i < 5; ++i) {
        auto children = ui->roster_tree->topLevelItem(i)->takeChildren();
        for (auto& x : children) delete x;
    }
}

void ArmyCreator::clear_army_tree() {
    ui->lord_tree->clear();
    ui->hero_tree->clear();
    ui->core_tree->clear();
    ui->special_tree->clear();
    ui->rare_tree->clear();
}

void ArmyCreator::clear_unit_info_box() {
    auto c = ui->unit_info_box->children();
    for (auto& x : c) delete x;
}

QTreeWidgetItem* ArmyCreator::current_item() const {
    switch (in_tree) {
    case InTree::LORD:
        return ui->lord_tree->currentItem();
    case InTree::HERO:
        return ui->hero_tree->currentItem();
    case InTree::CORE:
        return ui->core_tree->currentItem();
    case InTree::SPECIAL:
        return ui->special_tree->currentItem();
    case InTree::RARE:
        return ui->rare_tree->currentItem();
    case InTree::ROSTER:
        return ui->roster_tree->currentItem();
    default: return nullptr;
    }
}

// option box selection slots

void ArmyCreator::change_unit_size(int value, bool master) {
    switch (current->base_unit_type()) {
    case BaseUnitType::MELEE_CHARACTER:
    case BaseUnitType::MAGE_CHARACTER:
        break;
    case BaseUnitType::NORMAL:
    {
        auto p = std::dynamic_pointer_cast<normal_unit>(current);
        try {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(current->id());
                p->change_size(static_cast<std::size_t>(value));
                army->update_on(current->id());
                ui->current_points_label->setText(QString("%1").arg(army->current_points()));
                update_unit_display(current_item(), ArmyTreeColumn::SIZE, false, false);
                update_validity_label();
            }
            else p->change_size(static_cast<std::size_t>(value));
        } catch (const std::invalid_argument&) {}
        break;
    }
    case BaseUnitType::MIXED:
    {
        auto p = std::dynamic_pointer_cast<mixed_unit>(current);
        auto curr_master_size = static_cast<int>(p->master_size());
        auto curr_slave_size = static_cast<int>(p->slave_size());
        try {
            if (enum_convert::in_army_trees(in_tree)) {
                army->take_snapshot_of(current->id());
                if (master) p->change_master_size(static_cast<std::size_t>(value));
                else p->change_slave_size(static_cast<std::size_t>(value));
                army->update_on(current->id());
                ui->current_points_label->setText(QString("%1").arg(army->current_points()));
                update_unit_display(current_item(), ArmyTreeColumn::SIZE, false, false);
                update_validity_label();
            } else {
                if (master) p->change_master_size(static_cast<std::size_t>(value));
                else p->change_slave_size(static_cast<std::size_t>(value));
            }
        } catch (const std::invalid_argument& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, tr("Error"), tr(e.what()));
            message_box.setFixedSize(500, 200);
            change_unit_size(master ? curr_master_size : curr_slave_size, master);
            // FIXME: below causes CTD
            /*ob->clear();
            ob->reset(current, in_tree);
            ob->reinitialise();*/
        }
        break;
    }
    default: break;
    }
}

void ArmyCreator::optional_weapon_selected(const std::string& name,
                                           WeaponType wt, ItemCategory ic, bool champion, bool master) {
    try {
        opt_sel->select_weapon(name, wt, ic, champion, master);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::WEAPONS, false, false);
            update_unit_display(current_item(), ArmyTreeColumn::ARMOUR, false, false);
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
    mib->reset(current);
    mib->reinitialise(ItemType::WEAPON);
}

void ArmyCreator::optional_armour_selected(const std::string& name,
                                           ArmourType at, ItemCategory ic, bool champion, bool master) {
    try {
        opt_sel->select_armour(name, at, ic, champion, master);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::ARMOUR, false, false);
            update_unit_display(current_item(), ArmyTreeColumn::WEAPONS, false, false);
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
    mib->reset(current);
    mib->reinitialise(ItemType::ARMOUR);
}

void ArmyCreator::optional_talisman_selected(const std::string& name, ItemCategory ic) {
    try {
        opt_sel->select_talisman(name, ic);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::TALISMAN, false, false);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current);
    mib->reinitialise(ItemType::TALISMAN);
}

void ArmyCreator::optional_enchanted_item_selected(const std::string& name, ItemCategory ic) {
    try {
        opt_sel->select_enchanted_item(name, ic);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::ENCHANTED, false, false);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current);
    mib->reinitialise(ItemType::ENCHANTED);
}

void ArmyCreator::optional_other_item_selected(const std::string& name, ItemCategory ic, bool checked) {
    try {
        opt_sel->select_other_item(name, ic, checked);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::EXTRAS, false, false);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current);
    mib->reinitialise(ItemType::OTHER);
}

void ArmyCreator::optional_banner_selected(const std::string& name, ItemCategory ic) {
    try {
        opt_sel->select_banner(name, ic);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::BANNER, false, false);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current);
    mib->reinitialise(ItemType::BANNER);
}

void ArmyCreator::optional_arcane_item_selected(const std::string& name, ItemCategory ic) {
    try {
        opt_sel->select_arcane_item(name, ic);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::ARCANE, false, false);
            update_validity_label();
        }
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
    mib->clear();
    mib->reset(current);
    mib->reinitialise(ItemType::ARCANE);
}

void ArmyCreator::optional_level_selected(short level) {
    try {
        opt_sel->select_mage_level(level);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::LEVEL, false, false);
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

void ArmyCreator::optional_lore_selected(const std::string& name, std::string to_remove) {
    try {
        opt_sel->select_mage_lore(name, to_remove);
        if (enum_convert::in_army_trees(in_tree))
            update_unit_display(current_item(), ArmyTreeColumn::LORE, false, false);
    } catch (const std::exception& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        ob->clear();
        ob->reset(current, in_tree);
        ob->reinitialise();
    }
}

void ArmyCreator::optional_mount_selected(const std::string& name) {
    try {
        opt_sel->select_mount(name);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::MOUNT, false, false);
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
}

void ArmyCreator::optional_mount_oco_extra_selected(const std::string& name) {
    try {
        opt_sel->select_mount_oco_extra(name);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::MOUNT, false, false);
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

void ArmyCreator::optional_mount_mc_extra_selected(const std::string& name, bool checked) {
    try {
        opt_sel->select_mount_mc_extra(name, checked);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::MOUNT, false, false);
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

void ArmyCreator::optional_command_selected(CommandGroup member, bool checked, bool master) {
    try {
        opt_sel->select_command(member, checked, master);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::COMMAND, false, false);
            if (member == CommandGroup::CHAMPION || member == CommandGroup::STANDARD_BEARER) {
                update_unit_display(current_item(), ArmyTreeColumn::WEAPONS, false, false);
                update_unit_display(current_item(), ArmyTreeColumn::ARMOUR, false, false);
                update_unit_display(current_item(), ArmyTreeColumn::EXTRAS, false, false);
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

void ArmyCreator::optional_oco_extra_selected(const std::string& name, bool champion) {
    try {
        opt_sel->select_oco_extra(name, champion);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::EXTRAS, false, false);
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

void ArmyCreator::optional_mc_extra_selected(const std::string& name, bool champion, bool checked) {
    try {
        opt_sel->select_mc_extra(name, champion, checked);
        if (enum_convert::in_army_trees(in_tree)) {
            ui->current_points_label->setText(QString("%1").arg(army->current_points()));
            update_unit_display(current_item(), ArmyTreeColumn::EXTRAS, false, false);
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

// TODO: rewrite, don't like this function at all - might be worth just adding the necessary UI
// elements to the .ui form and updating them here
void ArmyCreator::initialise_unit_info_box() {
    if (current == nullptr) return;
    ui->unit_info_box->setTitle(tr(current->name().data()));
    // overall layout for unit info box
    QVBoxLayout* vbox = new QVBoxLayout;
    // add points label widget in a frame
    {
        QFrame* f = new QFrame();
        QHBoxLayout* points_label_layout = new QHBoxLayout;
        QLabel* pre_points_label;
        QLabel* points_label;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            pre_points_label = new QLabel(tr("Points:"));
            points_label = new QLabel(
                QString(tools::points_str(p->handle_->points(), p->base_unit_type()).data())
            );
            break;
        }
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
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
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
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
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
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
        stats_table->setMaximumWidth(static_cast<int>(0.8*ui->unit_info_box->width()));
        stats_table->setMaximumHeight(49);
        vbox->addWidget(stats_table);
    }
    // special rules
    {
        QLabel* special_rules_label = new QLabel();
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
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
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
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

void ArmyCreator::clear() {
    army->clear();
    clear_points_displays();
    current.reset();
    id_counter = 0;
    clear_roster_tree();
    clear_army_tree();
    /*for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree, ui->lord_tree, ui->hero_tree,
         ui->core_tree, ui->special_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }*/
    clear_unit_info_box();
    ob->clear();
    mib->clear();
    ui->item_description_groupbox->setTitle(tr("Item Description"));
    ui->item_description_label->setText(tr(""));
    in_tree = InTree::NEITHER;
    ui->magic_items_combobox->clear();
    update_validity_label();
}

void ArmyCreator::clear_points_displays() {
    ui->current_points_label->setText(QString("%1").arg(static_cast<double>(0.0)));
}

void ArmyCreator::setup_magic_items_combobox() {
    if (race != Faction::DWARFS)
        ui->magic_items_combobox->addItem(QString("Common"), QVariant(static_cast<int>(ItemCategory::COMMON)));
    if (!st->magic_items_name().empty())
        ui->magic_items_combobox->addItem(
            QString(st->magic_items_name().data()), QVariant(static_cast<int>(ItemCategory::MAGIC))
        );
    if (!st->faction_items_name().empty())
        ui->magic_items_combobox->addItem(
            QString(st->faction_items_name().data()), QVariant(static_cast<int>(ItemCategory::FACTION))
        );
}

void ArmyCreator::change_faction(Faction faction) {
    if (faction == race) return;
    try {
        st->reset(faction);
        army->switch_faction(faction);
        clear();
        populate_roster_tree();
        race = faction;
        mib->reset_category(ItemCategory::NONE);
        setup_magic_items_combobox();
    }
    catch (const std::runtime_error& e) {
        ui->faction_combobox->setCurrentText(QString::fromStdString(enum_convert::FACTION_TO_STRING.at(race)));
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
    }
}

void ArmyCreator::on_faction_combobox_currentTextChanged(const QString& faction) {
    change_faction(enum_convert::STRING_TO_FACTION.at(faction.toStdString()));
}

void ArmyCreator::on_magic_items_combobox_currentTextChanged(const QString& ic_select) {
    (void)(ic_select);
    if (mib != nullptr) {
        auto v = ui->magic_items_combobox->currentData().toInt();
        mib->clear();
        mib->reset(current);
        mib->reset_category(static_cast<ItemCategory>(v));
        mib->reinitialise(ItemType::WEAPON);
    }
}

void ArmyCreator::on_points_limit_spinbox_valueChanged(double pts) {
    army->change_points_limit(pts);
    update_validity_label();
}

// tree item changed or activated slots

// TODO: fix issues:
//         1) CTD when changing item in roster tree after failing to switch to another army
//         2) reset previously highlighted unit if we were already in the roster tree so that
//            it doesn't think that a magic item has been taken
void ArmyCreator::on_roster_tree_currentItemChanged(QTreeWidgetItem *curr, QTreeWidgetItem *previous) {
    (void)(previous);
    if (curr == nullptr) return;
    std::string name = curr->text(0).toStdString();
    if (name == "Lords" || name == "Heroes" || name == "Core" ||
            name == "Special" || name == "Rare") {
        ui->add_button->setEnabled(false);
        ui->duplicate_button->setEnabled(false);
        ui->remove_button->setEnabled(false);
        ui->set_general_button->setEnabled(false);
        return;
    }
    for (auto tree : std::vector<QTreeWidget*>{ui->lord_tree, ui->hero_tree,
         ui->core_tree, ui->special_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    clear_unit_info_box();
    ob->clear();
    mib->clear();
    ui->item_description_groupbox->setTitle(tr("Item Description"));
    ui->item_description_label->setText(tr(""));
    // do logic and gui changes
    st->change_selection(name);
    current = st->selected();
    in_tree = InTree::ROSTER;
    initialise_unit_info_box();
    opt_sel->reset(current, in_tree);
    ob->reset(current, in_tree);
    ob->reinitialise();
    mib->reset(current);
    mib->reinitialise(ItemType::WEAPON);
    ui->add_button->setEnabled(true);
    update_budget_label();
    // disable army tree related buttons
    ui->duplicate_button->setEnabled(false);
    ui->remove_button->setEnabled(false);
    ui->set_general_button->setEnabled(false);
    const int height_div = static_cast<int>(static_cast<double>(height())/40);
    if (current->base()->total_item_budget().points > 0.0 || current->base()->magic_banner_budget() > 0.0) {
        ui->lower_horizontal_splitter->show();
        ui->main_vertical_splitter->setSizes(
            QList<int>() << height_div
                         << 29*height_div
                         << 12*height_div
        );
    }
    else {
        ui->lower_horizontal_splitter->hide();
        ui->main_vertical_splitter->setSizes(
            QList<int>() << height_div
                         << 30*height_div
                         << 0
        );
    }
}

void ArmyCreator::on_roster_tree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    if (item == nullptr) return;
    std::string name = item->text(column).toStdString();
    if (name == "Lords" || name == "Heroes" || name == "Core" ||
            name == "Special" || name == "Rare") return;
    if (st->selected()->name() != name) st->change_selection(name);
    on_add_button_clicked();
}

bool ArmyCreator::army_trees_itemchanged(QTreeWidgetItem* curr) {
    if (curr == nullptr) return false;
    clear_unit_info_box();
    ob->clear();
    mib->clear();
    ui->item_description_groupbox->setTitle(tr("Item Description"));
    ui->item_description_label->setText(tr(""));
    // get the name and id of the selected unit
    std::string name = curr->text(0).toStdString();
    int id = curr->data(0, Qt::UserRole).toInt();
    current = army->get_unit(id);
    initialise_unit_info_box();
    opt_sel->reset(current, in_tree);
    ob->reset(current, in_tree);
    ob->reinitialise();
    mib->reset(current);
    mib->reinitialise(ItemType::WEAPON);
    ui->remove_button->setEnabled(true);
    ui->duplicate_button->setEnabled(true);
    if (in_tree == InTree::LORD || in_tree == InTree::HERO)
        ui->set_general_button->setEnabled(true);
    else ui->set_general_button->setEnabled(false);
    update_budget_label();
    const int height_div = static_cast<int>(static_cast<double>(height())/40);
    if (current->base()->total_item_budget().points > 0.0 || current->base()->magic_banner_budget() > 0.0) {
        ui->lower_horizontal_splitter->show();
        ui->main_vertical_splitter->setSizes(
            QList<int>() << height_div
                         << 29*height_div
                         << 12*height_div
        );
    }
    else {
        ui->lower_horizontal_splitter->hide();
        ui->main_vertical_splitter->setSizes(
            QList<int>() << height_div
                         << 30*height_div
                         << 0
        );
    }
    return true;
}

void ArmyCreator::on_lord_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    in_tree = InTree::LORD;
    if (current == previous || !army_trees_itemchanged(current)) return;
    for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree,
            ui->hero_tree, ui->core_tree, ui->special_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    ui->lord_tree->setCurrentItem(current);
}

void ArmyCreator::on_hero_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    in_tree = InTree::HERO;
    if (current == previous || !army_trees_itemchanged(current)) return;
    for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree,
            ui->lord_tree, ui->core_tree, ui->special_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    ui->hero_tree->setCurrentItem(current);
}

void ArmyCreator::on_core_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    in_tree = InTree::CORE;
    if (current == previous || !army_trees_itemchanged(current)) return;
    for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree,
            ui->lord_tree, ui->hero_tree, ui->special_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    ui->core_tree->setCurrentItem(current);
}

void ArmyCreator::on_special_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    in_tree = InTree::SPECIAL;
    if (current == previous || !army_trees_itemchanged(current)) return;
    for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree,
            ui->lord_tree, ui->hero_tree, ui->core_tree, ui->rare_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    ui->special_tree->setCurrentItem(current);
}

void ArmyCreator::on_rare_tree_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    (void)(previous);
    in_tree = InTree::RARE;
    if (current == previous || !army_trees_itemchanged(current)) return;
    for (auto tree : std::vector<QTreeWidget*>{ui->roster_tree,
            ui->lord_tree, ui->hero_tree, ui->core_tree, ui->special_tree}) {
        tree->clearSelection();
        tree->setCurrentItem(nullptr, 0, QItemSelectionModel::SelectionFlag::Clear);
    }
    ui->rare_tree->setCurrentItem(current);
}

// army_tree modifying button slots

void ArmyCreator::on_add_button_clicked() {
    try { st->add_unit_to_army_list(id_counter++); }
    catch (const std::invalid_argument& e) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr(e.what()));
        message_box.setFixedSize(500, 200);
        return;
    }
    ui->current_points_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    std::shared_ptr<unit> u = st->selected();
    // assign id
    QVariant v(u->id());
    item->setData(0, Qt::UserRole, v);
    update_unit_display(item, ArmyTreeColumn::ALL, true, false);
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
    mib->reset(current);
    mib->reinitialise(ItemType::WEAPON);
    update_validity_label();
}

void ArmyCreator::on_duplicate_button_clicked() {
    QTreeWidgetItem* curr_item = current_item();
    auto u = army->get_unit(curr_item->data(0, Qt::UserRole).toInt());
    if (u->has_non_duplicable_items()) {
        QMessageBox message_box;
        message_box.critical(nullptr, tr("Error"), tr("Cannot duplicate a unit with magic item(s)!"));
        message_box.setFixedSize(500, 200);
        return;
    }
    if (u->is_character()) {
        if (std::dynamic_pointer_cast<base_character_unit>(u->base())->unique()) {
            QMessageBox message_box;
            message_box.critical(nullptr, tr("Error"), tr("Cannot duplicate a special character!"));
            message_box.setFixedSize(500, 200);
            return;
        }
        if (std::dynamic_pointer_cast<character_unit>(u)->is_bsb()) {
            QMessageBox message_box;
            message_box.critical(nullptr, tr("Error"), tr("Cannot duplicate the Battle Standard Bearer!"));
            message_box.setFixedSize(500, 200);
            return;
        }
    }
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
    ui->current_points_label->setText(QString("%1").arg(army->current_points()));
    QTreeWidgetItem* item = new QTreeWidgetItem();
    QVariant v(cpy_ptr->id());
    item->setData(0, Qt::UserRole, v);
    update_unit_display(item, ArmyTreeColumn::ALL, true, true);
    ob->clear();
    ob->reset(army->get_unit(id_counter), in_tree);
    ob->reinitialise();
    mib->clear();
    mib->reset(army->get_unit(id_counter));
    mib->reinitialise(ItemType::WEAPON);
    update_validity_label();
}

void ArmyCreator::on_remove_button_clicked() {
    QTreeWidgetItem* item = current_item();
    if (item == nullptr) return;
    QVariant v = item->data(0, Qt::UserRole);
    int id = v.toInt();
    UnitType unit_type = army->get_unit(id)->unit_type();
    army->remove_unit(id);
    ui->current_points_label->setText(QString("%1").arg(army->current_points()));
    int col_val = 0;
    if (current->is_character())
        col_val = static_cast<int>(CharacterTreeColumn::POINTS);
    else
        col_val = static_cast<int>(UnitTreeColumn::POINTS);
    switch (unit_type) {
    case UnitType::LORD:
        ui->lord_tree->headerItem()->setText(
            col_val,
            QString("Points (%1)").arg(tools::points_str(army->lord_points()).data())
        );
        break;
    case UnitType::HERO:
        ui->hero_tree->headerItem()->setText(
            col_val,
            QString("Points (%1)").arg(tools::points_str(army->hero_points()).data())
        );
        break;
    case UnitType::CORE:
        ui->core_tree->headerItem()->setText(
            col_val,
            QString("Points (%1)").arg(tools::points_str(army->core_points()).data())
        );
        break;
    case UnitType::SPECIAL:
        ui->special_tree->headerItem()->setText(
            col_val,
            QString("Points (%1)").arg(tools::points_str(army->special_points()).data())
        );
        break;
    case UnitType::RARE:
        ui->rare_tree->headerItem()->setText(
            col_val,
            QString("Points (%1)").arg(tools::points_str(army->rare_points()).data())
        );
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
    clear_points_displays();
    clear_army_tree();
    ob->clear();
    mib->clear();
    clear_unit_info_box();
    update_validity_label();
    ui->item_description_groupbox->setTitle(tr("Item Description"));
    ui->item_description_label->setText(tr(""));
    // reset point displays for each tree
    ui->lord_tree->headerItem()->setText(
        static_cast<int>(CharacterTreeColumn::POINTS),
        QString("Points (0)")
    );
    ui->hero_tree->headerItem()->setText(
        static_cast<int>(CharacterTreeColumn::POINTS),
        QString("Points (0)")
    );
    ui->core_tree->headerItem()->setText(
        static_cast<int>(UnitTreeColumn::POINTS),
        QString("Points (0)")
    );
    ui->special_tree->headerItem()->setText(
        static_cast<int>(UnitTreeColumn::POINTS),
        QString("Points (0)")
    );
    ui->rare_tree->headerItem()->setText(
        static_cast<int>(UnitTreeColumn::POINTS),
        QString("Points (0)")
    );
}

void ArmyCreator::on_set_general_button_clicked() {
    army->set_as_general(current->id());
}

// TODO: split some common code (with non char version) into different methods
void ArmyCreator::update_character_unit_display(
    QTreeWidgetItem* item,
    CharacterTreeColumn column,
    bool adding,
    bool copying
        ) {
    if (item == nullptr) {
        QMessageBox error_box;
        error_box.critical(nullptr, tr("Error"), tr("Failed to update unit display - null unit detected!"));
        error_box.setFixedSize(500, 200);
        return;
    }
    std::shared_ptr<unit> u;
    if (adding) {
        if (copying) u = army->get_unit(item->data(0, Qt::UserRole).toInt());
        else u = st->selected();
    }
    else u = army->get_unit(item->data(0, Qt::UserRole).toInt());
    if (!u->is_character()) {
        QMessageBox error_box;
        error_box.critical(nullptr, tr("Error"), tr("Failed to update character unit display!"));
        error_box.setFixedSize(500, 200);
        return;
    }
    const int col_val = static_cast<int>(column);
    switch (column) {
    case CharacterTreeColumn::NAME:
    {
        item->setText(col_val, QString::fromStdString(u->full_name()));
        break;
    }
    case CharacterTreeColumn::MOUNT:
    {
        auto mnt_name = std::get<0>(u->mnt());
        if (!std::get<2>(u->mnt()).first.empty())
            mnt_name += "\n    " + std::get<2>(u->mnt()).first;
        for (const auto& x : std::get<3>(u->mnt()))
            mnt_name += "\n    " + x.first;
        item->setText(col_val, QString::fromStdString(mnt_name));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    }
    case CharacterTreeColumn::LEVEL:
        if (u->is_mage()) {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(u);
            item->setText(col_val, QString("%1").arg(p->level()));
        } else item->setText(col_val, tr("None"));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    case CharacterTreeColumn::LORE:
        if (u->is_mage()) {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(u);
            std::string lores = "";
            if (!p->lores().empty())
                lores = std::accumulate(std::next(std::begin(p->lores())), std::end(p->lores()),
                                                *std::begin(p->lores()), [](const auto& x, const auto& y) {
                return x + ", " + y;
                });
            item->setText(col_val, QString::fromStdString(lores));
        } else item->setText(col_val, tr(""));
        break;
    case CharacterTreeColumn::WEAPONS:
    {
        std::string weapons_str = "";
        auto weapons = u->weapons();
        auto melee = weapons.find(WeaponType::MELEE);
        auto ballistic = weapons.find(WeaponType::BALLISTIC);
        if (melee != std::end(weapons))
            weapons_str += std::get<1>(melee->second);
        if (ballistic != std::end(weapons)) {
            if (melee != std::end(weapons)) weapons_str += '\n';
            weapons_str += std::get<1>(ballistic->second);
        }
        item->setText(col_val, QString::fromStdString(weapons_str));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);;
        break;
    }
    case CharacterTreeColumn::ARMOUR:
    {
        std::string armour_str = "";
        auto armour = u->armour();
        auto body = armour.find(ArmourType::ARMOUR);
        auto shield = armour.find(ArmourType::SHIELD);
        auto helmet = armour.find(ArmourType::HELMET);
        if (body != std::end(armour))
            armour_str += std::get<1>(body->second);
        if (shield != std::end(armour)) {
            if (!armour_str.empty()) armour_str += '\n';
            armour_str += std::get<1>(shield->second);
        }
        if (helmet != std::end(armour)) {
            if (!armour_str.empty()) armour_str += '\n';
            armour_str += std::get<1>(helmet->second);
        }
        item->setText(col_val, QString::fromStdString(armour_str));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    }
    case CharacterTreeColumn::TALISMAN:
        item->setText(col_val, QString::fromStdString(
            std::dynamic_pointer_cast<character_unit>(u)->talisman().first
        ));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    case CharacterTreeColumn::ENCHANTED:
        item->setText(col_val, QString::fromStdString(
            std::dynamic_pointer_cast<character_unit>(u)->enchanted_item().first
        ));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    case CharacterTreeColumn::ARCANE:
        if (u->is_mage()) {
            item->setText(col_val, QString::fromStdString(
                std::dynamic_pointer_cast<mage_character_unit>(u)->arcane_item().first
            ));
            update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        }
        break;
    case CharacterTreeColumn::BANNER:
        item->setText(col_val, QString::fromStdString(u->magic_banner().first));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    case CharacterTreeColumn::EXTRAS:
    {
        std::string extras_str = "";
        auto oco_extra = u->oco_extra();
        extras_str += oco_extra.first;
        auto mc_extras = u->mc_extras();
        for (const auto& extra : mc_extras) {
            if (!extras_str.empty()) extras_str += '\n';
            extras_str += extra.first;
        }
        auto p = std::dynamic_pointer_cast<character_unit>(u);
        auto item_extras = p->item_extras();
        for (const auto& extra : item_extras) {
            if (!extras_str.empty()) extras_str += '\n';
            extras_str += extra.first;
        }
        item->setText(col_val, QString::fromStdString(extras_str));
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        break;
    }
    case CharacterTreeColumn::POINTS:
        item->setText(col_val, QString("%1").arg(u->points()));
        switch (u->unit_type()) {
        case UnitType::LORD:
            if (army->lord_no_contrib_points() > 0.0)
                ui->lord_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1 (%2)]").arg(
                        tools::points_str(army->lord_points()).data(),
                        tools::points_str(army->lord_points() + army->lord_no_contrib_points()).data()
                    )
                );
            else
                ui->lord_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1]").arg(tools::points_str(army->lord_points()).data())
                );
            break;
        case UnitType::HERO:
            if (army->hero_no_contrib_points() > 0.0)
                ui->hero_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1 (%2)]").arg(
                        tools::points_str(army->hero_points()).data(),
                        tools::points_str(army->hero_points() + army->hero_no_contrib_points()).data()
                    )
                );
            else
                ui->hero_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1]").arg(tools::points_str(army->hero_points()).data())
                );
            break;
        default: break;
        }
        break;
    case CharacterTreeColumn::ALL:
        update_character_unit_display(item, CharacterTreeColumn::NAME, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::MOUNT, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::LEVEL, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::LORE, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::WEAPONS, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::ARMOUR, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::TALISMAN, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::ENCHANTED, adding, copying);
        if (u->is_mage()) update_character_unit_display(item, CharacterTreeColumn::ARCANE, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::BANNER, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::EXTRAS, adding, copying);
        update_character_unit_display(item, CharacterTreeColumn::POINTS, adding, copying);
        if (adding) {
            switch (u->unit_type()) {
            case UnitType::LORD:
                ui->lord_tree->addTopLevelItem(item);
                break;
            case UnitType::HERO:
                ui->hero_tree->addTopLevelItem(item);
                break;
            default:
                delete item;
                break;
            }
        }
        break;
    }
}

void ArmyCreator::update_noncharacter_unit_display(
    QTreeWidgetItem* item,
    UnitTreeColumn column,
    bool adding,
    bool copying
        ) {
    if (item == nullptr) {
        QMessageBox error_box;
        error_box.critical(nullptr, tr("Error"), tr("Failed to update unit display - null unit detected!"));
        error_box.setFixedSize(500, 200);
        return;
    }
    std::shared_ptr<unit> u;
    if (adding) {
        if (copying) u = army->get_unit(item->data(0, Qt::UserRole).toInt());
        else u = st->selected();
    }
    else u = army->get_unit(item->data(0, Qt::UserRole).toInt());
    if (u->is_character()) {
        QMessageBox error_box;
        error_box.critical(nullptr, tr("Error"), tr("Failed to update unit display!"));
        error_box.setFixedSize(500, 200);
        return;
    }
    const int col_val = static_cast<int>(column);
    switch (column) {
    case UnitTreeColumn::NAME:
    {
        item->setText(col_val, QString::fromStdString(u->full_name()));
        break;
    }
    case UnitTreeColumn::MOUNT:
    {
        auto mnt_name = std::get<0>(u->mnt());
        if (!std::get<2>(u->mnt()).first.empty())
            mnt_name += "\n    " + std::get<2>(u->mnt()).first;
        for (const auto& x : std::get<3>(u->mnt()))
            mnt_name += "\n    " + x.first;
        item->setText(col_val, QString::fromStdString(mnt_name));
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    }
    case UnitTreeColumn::SIZE:
        switch (u->base_unit_type()) {
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            item->setText(col_val, QString("%1").arg(p->size()));
            break;
        }
        case BaseUnitType::MIXED:
        {
            auto p = std::dynamic_pointer_cast<mixed_unit>(u);
            auto msize = p->master_size();
            auto ssize = p->slave_size();
            auto mname = p->handle->master_name();
            auto sname = p->handle->slave_name();
            std::string text = std::to_string(msize) + ' ' + mname + (msize > 1 ? "s\n" : "\n")
                    + std::to_string(ssize) + ' ' + sname + "\n";
            item->setText(col_val, QString::fromStdString(text));
            break;
        }
        default: break;
        }
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    case UnitTreeColumn::WEAPONS:
    {
        std::string weapons_str = "";
        auto weapons = u->weapons();
        auto melee = weapons.find(WeaponType::MELEE);
        auto ballistic = weapons.find(WeaponType::BALLISTIC);
        if (melee != std::end(weapons))
            weapons_str += std::get<1>(melee->second);
        if (ballistic != std::end(weapons)) {
            if (melee != std::end(weapons)) weapons_str += '\n';
            weapons_str += std::get<1>(ballistic->second);
        }
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto cweapons = u->weapons();
            auto cmelee = cweapons.find(WeaponType::MELEE);
            auto cballistic = cweapons.find(WeaponType::BALLISTIC);
            // only display champion weapon if it is different from default unit member weapon
            if (cmelee != std::end(cweapons)
                    && std::get<1>(cmelee->second) != std::get<1>(melee->second)) {
                if (melee != std::end(weapons) || ballistic != std::end(weapons))
                    weapons_str += '\n';
                weapons_str += std::get<1>(cmelee->second) + " (Champion)";
            }
            if (cballistic != std::end(cweapons)
                    && std::get<1>(cballistic->second) != std::get<1>(ballistic->second)) {
                if (melee != std::end(weapons) || ballistic != std::end(weapons)
                        || cmelee != std::end(cweapons)) weapons_str += '\n';
                weapons_str += std::get<1>(cballistic->second) + " (Champion)";
            }
        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(col_val, QString::fromStdString(weapons_str));
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    }
    case UnitTreeColumn::ARMOUR:
    {
        std::string armour_str = "";
        auto armour = u->armour();
        auto body = armour.find(ArmourType::ARMOUR);
        auto shield = armour.find(ArmourType::SHIELD);
        auto helmet = armour.find(ArmourType::HELMET);
        if (body != std::end(armour))
            armour_str += std::get<1>(body->second);
        if (shield != std::end(armour)) {
            if (!armour_str.empty()) armour_str += '\n';
            armour_str += std::get<1>(shield->second);
        }
        if (helmet != std::end(armour)) {
            if (!armour_str.empty()) armour_str += '\n';
            armour_str += std::get<1>(helmet->second);
        }
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto carmour = u->armour();
            auto cbody = carmour.find(ArmourType::ARMOUR);
            auto cshield = carmour.find(ArmourType::SHIELD);
            auto chelmet = carmour.find(ArmourType::HELMET);
            // only display champion armour if it is different from default unit member armour
            if (cbody != std::end(carmour)
                    && std::get<1>(cbody->second) != std::get<1>(body->second))
                armour_str += std::get<1>(cbody->second);
            if (cshield != std::end(carmour)
                    && std::get<1>(cshield->second) != std::get<1>(shield->second)) {
                if (!armour_str.empty()) armour_str += '\n';
                armour_str += std::get<1>(cshield->second);
            }
            if (chelmet != std::end(carmour)
                    && std::get<1>(chelmet->second) != std::get<1>(helmet->second)) {
                if (!armour_str.empty()) armour_str += '\n';
                armour_str += std::get<1>(chelmet->second);
            }
        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(col_val, QString::fromStdString(armour_str));
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    }
    case UnitTreeColumn::COMMAND:
        switch (u->base_unit_type()) {
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(u);
            QString command_str;
            update_unit_command_display_helper(p->command(), command_str);
            item->setText(col_val, command_str);
            break;
        }
        case BaseUnitType::MIXED:
        {
            // TODO: implement
            break;
        }
        default: break;
        }
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    case UnitTreeColumn::BANNER:
        item->setText(col_val, QString::fromStdString(u->magic_banner().first));
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    case UnitTreeColumn::EXTRAS:
    {
        std::string extras_str = "";
        auto oco_extra = u->oco_extra();
        extras_str += oco_extra.first;
        auto mc_extras = u->mc_extras();
        for (const auto& extra : mc_extras) {
            if (!extras_str.empty()) extras_str += '\n';
            extras_str += extra.first;
        }
        if (u->switch_model_select(ModelSelect::CHAMPION)) {
            auto champ_oco_extra = u->oco_extra();
            if (!extras_str.empty()) extras_str += '\n';
            extras_str += champ_oco_extra.first;
            auto champ_mc_extras = u->mc_extras();
            for (const auto& extra : champ_mc_extras) {
                if (!extras_str.empty()) extras_str += '\n';
                extras_str += extra.first;
            }
        }
        u->switch_model_select(ModelSelect::DEFAULT);
        item->setText(col_val, QString::fromStdString(extras_str));
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        break;
    }
    case UnitTreeColumn::POINTS:
    {
        item->setText(col_val, QString("%1").arg(u->points()));
        switch (u->unit_type()) {
        case UnitType::CORE:
            if (army->core_no_contrib_points() > 0.0)
                ui->core_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1 (%2)]").arg(
                        tools::points_str(army->core_points()).data(),
                        tools::points_str(army->core_points() + army->core_no_contrib_points()).data()
                    )
                );
            else
                ui->core_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1]").arg(tools::points_str(army->core_points()).data())
                );
            break;
        case UnitType::SPECIAL:
            if (army->special_no_contrib_points() > 0.0)
                ui->special_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1 (%2)]").arg(
                        tools::points_str(army->special_points()).data(),
                        tools::points_str(army->special_points() + army->special_no_contrib_points()).data()
                    )
                );
            else
                ui->special_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1]").arg(tools::points_str(army->special_points()).data())
                );
            break;
        case UnitType::RARE:
            if (army->rare_no_contrib_points() > 0.0)
                ui->rare_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1 (%2)]").arg(
                        tools::points_str(army->rare_points()).data(),
                        tools::points_str(army->rare_points() + army->rare_no_contrib_points()).data()
                    )
                );
            else
                ui->rare_tree->headerItem()->setText(
                    col_val,
                    QString("Points [%1]").arg(tools::points_str(army->rare_points()).data())
                );
            break;
        default:
            break;
        }
        break;
    }
    case UnitTreeColumn::ALL:
        update_noncharacter_unit_display(item, UnitTreeColumn::NAME, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::SIZE, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::MOUNT, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::WEAPONS, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::ARMOUR, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::BANNER, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::COMMAND, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::EXTRAS, adding, copying);
        update_noncharacter_unit_display(item, UnitTreeColumn::POINTS, adding, copying);
        if (adding) {
            switch (u->unit_type()) {
            case UnitType::CORE:
                ui->core_tree->addTopLevelItem(item);
                break;
            case UnitType::SPECIAL:
                ui->special_tree->addTopLevelItem(item);
                break;
            case UnitType::RARE:
                ui->rare_tree->addTopLevelItem(item);
                break;
            default:
                delete item;
                break;
            }
        }
        break;
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
        command_str += QString("%1").arg(
                          std::get<0>(musician_it->second).data()
                       );
    }
    if (sb_it != command.end()) {
        if (!command_str.isEmpty()) command_str += QString("\n");
        command_str += QString("%1").arg(
                          std::get<0>(sb_it->second).data()
                       );
    }
    if (champion_it != command.end()) {
        if (!command_str.isEmpty()) command_str += QString("\n");
        command_str += QString("%1").arg(
                          std::get<0>(champion_it->second).data()
                       );
    }
}

void ArmyCreator::update_unit_display(QTreeWidgetItem* item, ArmyTreeColumn col, bool adding, bool copying) {
    if (item == nullptr) {
        // somehow item has become null when switching between army trees so lets
        // re-update all the units in the current tree to ensure all model <-> ui
        // association remains consistent
        update_unit_displays();
        return;
    }
    std::shared_ptr<unit> u;
    if (adding) {
        if (copying) u = army->get_unit(item->data(0, Qt::UserRole).toInt());
        else u = st->selected();
    }
    else u = army->get_unit(item->data(0, Qt::UserRole).toInt());
    if (u->is_character())
        update_character_unit_display(item, enum_convert::ATC_TO_CTC.at(col), adding, copying);
    else
        update_noncharacter_unit_display(item, enum_convert::ATC_TO_UTC.at(col), adding, copying);
}

void ArmyCreator::update_unit_displays() {
    switch (in_tree) {
    case InTree::ROSTER:
    case InTree::ARMY:
    case InTree::NEITHER:
        break;
    case InTree::LORD:
        for (int i = 0; i < ui->lord_tree->topLevelItemCount(); ++i)
            update_unit_display(ui->lord_tree->topLevelItem(i), ArmyTreeColumn::ALL, false, false);
        break;
    case InTree::HERO:
        for (int i = 0; i < ui->hero_tree->topLevelItemCount(); ++i)
            update_unit_display(ui->hero_tree->topLevelItem(i), ArmyTreeColumn::ALL, false, false);
        break;
    case InTree::CORE:
        for (int i = 0; i < ui->core_tree->topLevelItemCount(); ++i)
            update_unit_display(ui->core_tree->topLevelItem(i), ArmyTreeColumn::ALL, false, false);
        break;
    case InTree::SPECIAL:
        for (int i = 0; i < ui->special_tree->topLevelItemCount(); ++i)
            update_unit_display(ui->special_tree->topLevelItem(i), ArmyTreeColumn::ALL, false, false);
        break;
    case InTree::RARE:
        for (int i = 0; i < ui->rare_tree->topLevelItemCount(); ++i)
            update_unit_display(ui->rare_tree->topLevelItem(i), ArmyTreeColumn::ALL, false, false);
        break;
    }
}

void ArmyCreator::setup_export_directories() {
    documents_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    if (documents_dir.isEmpty()) {
        QMessageBox error_box;
        error_box.critical(nullptr, tr("Error"), tr("Could not find Documents directory... disabling army exporting!"));
        error_box.setFixedSize(500, 200);
        ui->export_button->setEnabled(false);
        return;
    }
    if (QStandardPaths::locate(
                    QStandardPaths::DocumentsLocation,
                    army_list_dir,
                    QStandardPaths::LocateOption::LocateDirectory
                ).isEmpty()) {
        QDir docs(documents_dir);
        docs.mkdir(army_list_dir);
    }
    for (const auto& x : enum_convert::STRING_TO_FACTION) {
        auto faction_str = QString::fromStdString(x.first);
        if (QStandardPaths::locate(
                    QStandardPaths::DocumentsLocation,
                    army_list_dir + '/' + faction_str,
                    QStandardPaths::LocateOption::LocateDirectory
                ).isEmpty()) {
            QDir faction_dir(documents_dir + QString("/WHFB_ArmyLists"));
            faction_dir.mkdir(faction_str);
        }
    }
}

void ArmyCreator::on_export_button_clicked() {
    QString str_stream;
    QTextStream out(&str_stream);
    // TODO: print title in red with [INVALID] if army list is invalid
    out <<  "<html>\n"
            "<head>\n"
            "<meta Content=\"Text/html; charset=Windows-1251\">\n"
            <<  QString("<title>%1</title>\n").arg("Title")
            <<  "</head>\n"
                "<style>"
                    "body {font-family: \"Moria Citadel\"; background-color: #FFFFFF;}\n"
                    << QString("thead {font-family: \"Constantia\"; font-size: %1px;}\n").arg(
                           army->point_limit() > 2000.0 ? "10" : "12"
                       )
                    << QString("td {font-family: \"Constantia\"; font-size: %1px;}\n").arg(
                           army->point_limit() > 2000.0 ? "8" : "10"
                       )
                << "</style>\n"
            "<body>\n"
            "<header>\n"
             << QString("<h2>A %1/%2 points %3 army list</h2>").arg(
                    tools::points_str(army->current_points()).data(),
                    tools::points_str(army->point_limit()).data(),
                    ui->faction_combobox->currentText()
                )
            << "</header>\n";
    out << QString::fromStdString(army->html_characters_table());
    out << "\n<br/>\n";
    out << QString::fromStdString(army->html_units_table());
    out << "</body>\n"
           "</html>\n";
    QTextDocument* document = new QTextDocument();
    document->setHtml(str_stream);
    QPrinter printer;
    printer.setResolution(96);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    QString file_name = QFileDialog::getSaveFileName(
        this,
        tr("Export army list to PDF"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("PDF (*.pdf)")
    );
    QMessageBox mb;
    if (file_name.isEmpty())
        mb.information(nullptr, tr("Write failure"), tr("Could not write to this location!"));
    else {
        printer.setOutputFileName(file_name);
        document->setPageSize(printer.pageRect().size());
        document->print(&printer);
    }
    mb.setFixedSize(500, 200);
    delete document;
}

void ArmyCreator::on_name_unit_button_clicked() {
    bool ok;
    QString assigned_name = QInputDialog::getText(this, tr("Assign a name"), tr("Name:"), QLineEdit::Normal, QString::fromStdString(current->name()), &ok);
    if (ok && !assigned_name.isEmpty()) {
        current->assign_name(assigned_name.toStdString());
        update_unit_display(current_item(), ArmyTreeColumn::NAME, false, false);
    }
}

void ArmyCreator::on_save_button_clicked() {
    QString file_name = QFileDialog::getSaveFileName(
        this,
        tr("Save army list"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Army files (*.army)")
    );
    if (file_name.isEmpty()) {
        QMessageBox mb;
        mb.information(nullptr, tr("Write failure"), tr("Could not write to this location!"));
        mb.setFixedSize(500, 200);
    }
    else
        army->save(file_name);
}

void ArmyCreator::on_load_button_clicked() {
    QString file_name = QFileDialog::getOpenFileName(
        this,
        tr("Load army list"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("Army files (*.army)")
    );
    // TODO make army_parser class (inheriting from file_parser) which loads from file_name
}
