#include "magicitembox.h"
#include "armycreator.h"

MagicItemBox::MagicItemBox(ArmyCreator* creator_, QTabWidget* box_, QGroupBox* descr_box_, QLabel* descr_label_) :
    creator(creator_), box(box_), descr_box(descr_box_), descr_label(descr_label_),
    in_tree(InTree::NEITHER), ic_selected(ItemClass::COMMON) {}

void MagicItemBox::clear() {
    /*auto c = box->children();
    for (auto& x : c) delete x;
    box->clear();*/
    box->clear();
    for (auto i = 0; i < box->count(); ++i)
        box->removeTab(i);
    current.reset();
}

void MagicItemBox::reset(std::shared_ptr<unit> current_, InTree in_tree_) {
    current = current_;
    in_tree = in_tree_;
}

void MagicItemBox::reset_category(ItemClass ic_selected_) {
    ic_selected = ic_selected_;
}

QGroupBox* MagicItemBox::make_items_tab(const std::unordered_map<std::string, item>& items,
                                        ItemType item_type) {
    if (items.empty()) return nullptr;
    auto opt_items = tools::magic_items_vec_of(items, item_type);
    if (opt_items.empty() || !std::count_if(
        std::begin(opt_items), std::end(opt_items), [current=this->current](const auto& x) {
            return x.second.allowed_units.empty() || x.second.allowed_units.count(current->name());
        }
    )) return nullptr;
    std::sort(
        std::begin(opt_items), std::end(opt_items),
        [](const auto& lhs, const auto& rhs) { return lhs.second.points > rhs.second.points; }
    );
    switch (item_type) {
    case ItemType::WEAPON:
        return make_weapons_tab(opt_items);
    case ItemType::ARMOUR:
        return make_armour_tab(opt_items);
    case ItemType::TALISMAN:
        return make_talismans_tab(opt_items);
    case ItemType::ENCHANTED:
        return make_enchanted_tab(opt_items);
    case ItemType::ARCANE:
        return make_arcane_tab(opt_items);
    case ItemType::OTHER:
        return make_other_tab(opt_items);
    case ItemType::BANNER:
        return make_banners_tab(opt_items);
    default: return nullptr;
    }
}

QGroupBox* MagicItemBox::make_weapons_tab(const std::vector<std::pair<std::string, item>>& opt_weapons) {
    std::unordered_map<WeaponType, std::tuple<ItemClass, std::string, double>> weapons;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        weapons = p->weapons();
        break;
    }
    default: return nullptr;
    }
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    // melee box
    QGroupBox* mbox = new QGroupBox(creator->tr("Melee"));
    // set-up frames and associated horizontal layouts for melee weapons box
    const auto max_per_row = 7;
    auto n_melee_adj = static_cast<std::size_t>(std::ceil(std::count_if(std::begin(opt_weapons), std::end(opt_weapons),
                                 [](const auto& x) { return x.second.weapon_type == WeaponType::MELEE; })
                       /static_cast<double>(max_per_row)));
    QVBoxLayout* mvlayout = new QVBoxLayout;
    std::vector<QFrame*> mhframes(n_melee_adj);
    for (auto& f : mhframes) f = new QFrame;
    std::vector<QHBoxLayout*> mhlayouts(n_melee_adj);
    for (auto& x : mhlayouts) x = new QHBoxLayout;
    // ranged box
    QGroupBox* rbox = new QGroupBox(creator->tr("Ranged"));
    QHBoxLayout* rhlayout = new QHBoxLayout;
    bool has_weapon_m = false;
    bool has_weapon_r = false;
    std::size_t count_m = 0U;
    for (const auto& w : opt_weapons) {
        if (w.second.allowed_units.size() && !w.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            if (w.second.points > p->handle_->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(w.second.points);
        std::string button_name = w.first + "_" +
                std::to_string(static_cast<int>(w.second.weapon_type)) + "_" +
                std::to_string(static_cast<int>(w.second.item_class)) +
                "_default_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr((w.first + " (" + pts_str + " pts)").data()));
        rb->setObjectName(QString::fromStdString(button_name));
        rb->setToolTip(creator->tr(w.second.description.data()));
        if (weapons.count(w.second.weapon_type)) {
            if (std::get<1>(weapons.at(w.second.weapon_type)) == w.first) {
                rb->setChecked(true);
                switch (w.second.weapon_type) {
                case WeaponType::MELEE: has_weapon_m = true; break;
                case WeaponType::BALLISTIC: has_weapon_r = true; break;
                default: break;
                }
                descr_box->setTitle(QString::fromStdString(w.first));
                descr_label->setText(QString::fromStdString(w.second.description));
            }
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_weapon_selected()));
        if (w.second.weapon_type == WeaponType::MELEE)
            mhlayouts[count_m++/max_per_row]->addWidget(rb);
        else if (w.second.weapon_type == WeaponType::BALLISTIC) rhlayout->addWidget(rb);
    }
    // melee none button
    QRadioButton* none_rb_m = new QRadioButton(creator->tr("None"));
    std::string none_rb_m_name = "None_opt_melee_default_radiobutton";
    none_rb_m->setObjectName(QString::fromStdString(none_rb_m_name));
    if (!has_weapon_m) none_rb_m->setChecked(true);
    creator->connect(none_rb_m, SIGNAL(clicked(bool)), creator, SLOT(optional_weapon_selected()));
    // finalise melee weapons box layout
    if (!mhlayouts.empty()) (*(--std::end(mhlayouts)))->addWidget(none_rb_m);
    for (auto l : mhlayouts) l->addStretch(1);
    for (auto i = 0U; i < mhframes.size(); ++i) {
        mhframes[i]->setLayout(mhlayouts[i]);
        mvlayout->addWidget(mhframes[i]);
    }
    mbox->setLayout(mvlayout);
    // ranged none button
    QRadioButton* none_rb_r = new QRadioButton(creator->tr("None"));
    none_rb_r->setObjectName(QString("None_opt_ranged_default_radiobutton"));
    if (!has_weapon_r) none_rb_r->setChecked(true);
    creator->connect(none_rb_r, SIGNAL(clicked(bool)), creator, SLOT(optional_weapon_selected()));
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

QGroupBox* MagicItemBox::make_armour_tab(const std::vector<std::pair<std::string, item>>& opt_armour) {
    std::unordered_map<ArmourType, std::tuple<ItemClass, std::string, double>> armour;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        armour = p->armour();
        // mages with no armour already present in their
        // default equipment cannot choose magic armour
        if (p->is_mage() && p->handle_->eq().armours().empty()) return nullptr;
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    // armour
    QGroupBox* abox = new QGroupBox(creator->tr("Body Armour"));
    QHBoxLayout* ahlayout = new QHBoxLayout;
    // shield
    QGroupBox* sbox = new QGroupBox(creator->tr("Shield"));
    QHBoxLayout* shlayout = new QHBoxLayout;
    // helmet
    QGroupBox* hbox = new QGroupBox(creator->tr("Helmet"));
    QHBoxLayout* hhlayout = new QHBoxLayout;
    bool has_body_armour = false;
    bool has_shield = false;
    bool has_helmet = false;
    for (const auto& a : opt_armour) {
        if (a.second.allowed_units.size() && !a.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            if (a.second.points > p->handle_->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(a.second.points);
        QRadioButton* rb = new QRadioButton(creator->tr((a.first + " (" + pts_str + " pts)").data()));
        rb->setToolTip(creator->tr(a.second.description.data()));
        std::string button_name = a.first + "_" +
                std::to_string(static_cast<int>(a.second.armour_type)) + "_" +
                std::to_string(static_cast<int>(a.second.item_class)) +
                "_default_radiobutton";
        rb->setObjectName(QString::fromStdString(button_name));
        if (armour.count(a.second.armour_type)) {
            if (std::get<1>(armour.at(a.second.armour_type)) == a.first) {
                rb->setChecked(true);
                switch (a.second.armour_type) {
                case ArmourType::ARMOUR: has_body_armour = true; break;
                case ArmourType::SHIELD: has_shield = true; break;
                case ArmourType::HELMET: has_helmet = true; break;
                default: break;
                }
                descr_box->setTitle(QString::fromStdString(a.first));
                descr_label->setText(QString::fromStdString(a.second.description));
            }
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
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
    QRadioButton* none_rb_a = new QRadioButton(creator->tr("None"));
    none_rb_a->setObjectName(QString("None_opt_armour_default_radiobutton"));
    if (!has_body_armour) none_rb_a->setChecked(true);
    creator->connect(none_rb_a, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
    ahlayout->addWidget(none_rb_a);
    ahlayout->addStretch(1);
    abox->setLayout(ahlayout);
    // shield
    QRadioButton* none_rb_s = new QRadioButton(creator->tr("None"));
    none_rb_s->setObjectName(QString("None_opt_shield_default_radiobutton"));
    if (!has_shield) none_rb_s->setChecked(true);
    creator->connect(none_rb_s, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
    shlayout->addWidget(none_rb_s);
    shlayout->addStretch(1);
    sbox->setLayout(shlayout);
    // helmet
    QRadioButton* none_rb_h = new QRadioButton(creator->tr("None"));
    none_rb_h->setObjectName(QString("None_opt_helmet_default_radiobutton"));
    if (!has_helmet) none_rb_h->setChecked(true);
    creator->connect(none_rb_h, SIGNAL(clicked(bool)), creator, SLOT(optional_armour_selected()));
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

QGroupBox* MagicItemBox::make_talismans_tab(const std::vector<std::pair<std::string, item>>& opt_talismans) {
    std::pair<std::string, std::pair<ItemClass, double>> talisman;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        talisman = p->talisman();
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    const auto max_per_row = 7;
    auto n_adj = static_cast<std::size_t>(std::ceil(opt_talismans.size()/static_cast<double>(max_per_row)));
    std::vector<QFrame*> frames(n_adj);
    for (auto& f : frames) f = new QFrame;
    std::vector<QHBoxLayout*> hlayouts(n_adj);
    for (auto& l : hlayouts) l = new QHBoxLayout;
    std::size_t count = 0U;
    bool has_talisman = false;
    for (const auto& t : opt_talismans) {
        if (t.second.allowed_units.size() && !t.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            if (t.second.points > p->handle_->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(t.second.points);
        std::string button_name = t.first + "_" +
                std::to_string(static_cast<int>(t.second.item_class)) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr((t.first + " (" + pts_str + " pts)").data()));
        rb->setObjectName(QString::fromStdString(button_name));
        rb->setToolTip(creator->tr(t.second.description.data()));
        if (talisman.first == t.first) {
            rb->setChecked(true);
            has_talisman = true;
            descr_box->setTitle(QString::fromStdString(t.first));
            descr_label->setText(QString::fromStdString(t.second.description));
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_talisman_selected()));
        try { hlayouts.at(count++/max_per_row)->addWidget(rb); }
        catch (const std::out_of_range& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, creator->tr("Error"), creator->tr(e.what()));
            message_box.setFixedSize(500, 200);
        }
    }
    // none button
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    none_rb->setObjectName(QString("None_talisman_radiobutton"));
    if (!has_talisman) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_talisman_selected()));
    if (!hlayouts.empty()) (*(--std::end(hlayouts)))->addWidget(none_rb);
    for (auto l : hlayouts) l->addStretch(1);
    for (auto i = 0U; i < frames.size(); ++i) {
        frames[i]->setLayout(hlayouts[i]);
        vlayout->addWidget(frames[i]);
    }
    box->setLayout(vlayout);
    return box;
}

QGroupBox* MagicItemBox::make_enchanted_tab(const std::vector<std::pair<std::string, item>>& opt_enchanted) {
    std::pair<std::string, std::pair<ItemClass, double>> enchanted;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        enchanted = p->enchanted_item();
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    const auto max_per_row = 7;
    auto n_adj = static_cast<std::size_t>(std::ceil(opt_enchanted.size()/static_cast<double>(max_per_row)));
    std::vector<QFrame*> frames(n_adj);
    for (auto& f : frames) f = new QFrame;
    std::vector<QHBoxLayout*> hlayouts(n_adj);
    for (auto& l : hlayouts) l = new QHBoxLayout;
    std::size_t count = 0U;
    bool has_enchanted = false;
    for (const auto& t : opt_enchanted) {
        if (t.second.allowed_units.size() && !t.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            if (t.second.points > p->handle_->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(t.second.points);
        std::string button_name = t.first + "_" +
                std::to_string(static_cast<int>(t.second.item_class)) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr((t.first + " (" + pts_str + " pts)").data()));
        rb->setObjectName(QString::fromStdString(button_name));
        rb->setToolTip(creator->tr(t.second.description.data()));
        if (enchanted.first == t.first) {
            rb->setChecked(true);
            has_enchanted = true;
            descr_box->setTitle(QString::fromStdString(t.first));
            descr_label->setText(QString::fromStdString(t.second.description));
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_enchanted_item_selected()));
        try { hlayouts.at(count++/max_per_row)->addWidget(rb); }
        catch (const std::out_of_range& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, creator->tr("Error"), creator->tr(e.what()));
            message_box.setFixedSize(500, 200);
        }
    }
    // none button
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    none_rb->setObjectName(QString("None_talisman_radiobutton"));
    if (!has_enchanted) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_enchanted_item_selected()));
    if (!hlayouts.empty()) (*(--std::end(hlayouts)))->addWidget(none_rb);
    for (auto l : hlayouts) l->addStretch(1);
    for (auto i = 0U; i < frames.size(); ++i) {
        frames[i]->setLayout(hlayouts[i]);
        vlayout->addWidget(frames[i]);
    }
    box->setLayout(vlayout);
    return box;
}

QGroupBox* MagicItemBox::make_arcane_tab(const std::vector<std::pair<std::string, item>>& opt_arcane) {
    std::pair<std::string, std::pair<ItemClass, double>> arcane;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
        arcane = p->arcane_item();
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    const auto max_per_row = 7;
    auto n_adj = static_cast<std::size_t>(std::ceil(opt_arcane.size()/static_cast<double>(max_per_row)));
    std::vector<QFrame*> frames(n_adj);
    for (auto& f : frames) f = new QFrame;
    std::vector<QHBoxLayout*> hlayouts(n_adj);
    for (auto& l : hlayouts) l = new QHBoxLayout;
    std::size_t count = 0U;
    bool has_arcane = false;
    for (const auto& t : opt_arcane) {
        if (t.second.allowed_units.size() && !t.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<mage_character_unit>(current);
            if (t.second.points > p->handle->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(t.second.points);
        std::string button_name = t.first + "_" +
                std::to_string(static_cast<int>(t.second.item_class)) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr((t.first + " (" + pts_str + " pts)").data()));
        rb->setObjectName(QString::fromStdString(button_name));
        rb->setToolTip(creator->tr(t.second.description.data()));
        if (arcane.first == t.first) {
            rb->setChecked(true);
            has_arcane = true;
            descr_box->setTitle(QString::fromStdString(t.first));
            descr_label->setText(QString::fromStdString(t.second.description));
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_arcane_item_selected()));
        try { hlayouts.at(count++/max_per_row)->addWidget(rb); }
        catch (const std::out_of_range& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, creator->tr("Error"), creator->tr(e.what()));
            message_box.setFixedSize(500, 200);
        }
    }
    // none button
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    none_rb->setObjectName(QString("None_arcane_radiobutton"));
    if (!has_arcane) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_arcane_item_selected()));
    if (!hlayouts.empty()) (*(--std::end(hlayouts)))->addWidget(none_rb);
    for (auto l : hlayouts) l->addStretch(1);
    for (auto i = 0U; i < frames.size(); ++i) {
        frames[i]->setLayout(hlayouts[i]);
        vlayout->addWidget(frames[i]);
    }
    box->setLayout(vlayout);
    return box;
}

QGroupBox* MagicItemBox::make_banners_tab(const std::vector<std::pair<std::string, item>>& opt_banners) {
    std::pair<std::string, std::pair<ItemClass, double>> banner;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        banner = (std::dynamic_pointer_cast<character_unit>(current))->magic_banner();
        break;
    }
    case BaseUnitType::NORMAL:
    {
        banner = (std::dynamic_pointer_cast<normal_unit>(current))->magic_banner();
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    const auto max_per_row = 5;
    auto n_adj = static_cast<std::size_t>(std::ceil(opt_banners.size()/static_cast<double>(max_per_row)));
    std::vector<QFrame*> frames(n_adj);
    for (auto& f : frames) f = new QFrame;
    std::vector<QHBoxLayout*> hlayouts(n_adj);
    for (auto& l : hlayouts) l = new QHBoxLayout;
    std::size_t count = 0U;
    bool has_banner = false;
    for (const auto& t : opt_banners) {
        if (t.second.allowed_units.size() && !t.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::NORMAL:
        {
            auto p = std::dynamic_pointer_cast<normal_unit>(current);
            if (t.second.points > p->handle->magic_banner_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(t.second.points);
        std::string button_name = t.first + "_" +
                std::to_string(static_cast<int>(t.second.item_class)) + "_radiobutton";
        QRadioButton* rb = new QRadioButton(creator->tr((t.first + " (" + pts_str + " pts)").data()));
        rb->setObjectName(QString::fromStdString(button_name));
        rb->setToolTip(creator->tr(t.second.description.data()));
        if (banner.first == t.first) {
            rb->setChecked(true);
            has_banner = true;
            descr_box->setTitle(QString::fromStdString(t.first));
            descr_label->setText(QString::fromStdString(t.second.description));
        }
        creator->connect(rb, SIGNAL(clicked(bool)), creator, SLOT(optional_banner_selected()));
        hlayouts[count++/max_per_row]->addWidget(rb);
    }
    // none button
    QRadioButton* none_rb = new QRadioButton(creator->tr("None"));
    none_rb->setObjectName(QString("None_arcane_radiobutton"));
    if (!has_banner) none_rb->setChecked(true);
    creator->connect(none_rb, SIGNAL(clicked(bool)), creator, SLOT(optional_banner_selected()));
    if (!hlayouts.empty()) (*(--std::end(hlayouts)))->addWidget(none_rb);
    for (auto l : hlayouts) l->addStretch(1);
    for (auto i = 0U; i < frames.size(); ++i) {
        frames[i]->setLayout(hlayouts[i]);
        vlayout->addWidget(frames[i]);
    }
    box->setLayout(vlayout);
    return box;
}

QGroupBox* MagicItemBox::make_other_tab(const std::vector<std::pair<std::string, item>>& opt_other) {
    std::unordered_map<std::string, std::pair<ItemClass, double>> others;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        auto p = std::dynamic_pointer_cast<character_unit>(current);
        others = p->item_extras();
        break;
    }
    default: return nullptr;
    }
    // overall
    QGroupBox* box = new QGroupBox();
    QVBoxLayout* vlayout = new QVBoxLayout;
    const auto max_per_row = 7;
    auto n_adj = static_cast<std::size_t>(std::ceil(opt_other.size()/static_cast<double>(max_per_row)));
    std::vector<QFrame*> frames(n_adj);
    for (auto& f : frames) f = new QFrame;
    std::vector<QHBoxLayout*> hlayouts(n_adj);
    for (auto& l : hlayouts) l = new QHBoxLayout;
    std::size_t count = 0U;
    for (const auto& t : opt_other) {
        if (t.second.allowed_units.size() && !t.second.allowed_units.count(current->name()))
            continue;
        switch (current->base_unit_type()) {
        case BaseUnitType::MAGE_CHARACTER:
        case BaseUnitType::MELEE_CHARACTER:
        {
            auto p = std::dynamic_pointer_cast<character_unit>(current);
            if (t.second.points > p->handle_->magic_item_budget()) continue;
            break;
        }
        default: break;
        }
        std::string pts_str = tools::points_str(t.second.points);
        std::string button_name = t.first + "_" +
                std::to_string(static_cast<int>(t.second.item_class)) + "_checkbox";
        QCheckBox* cb = new QCheckBox(creator->tr((t.first + " (" + pts_str + " pts)").data()));
        cb->setObjectName(QString::fromStdString(button_name));
        cb->setToolTip(creator->tr(t.second.description.data()));
        if (others.count(t.first)) cb->setChecked(true);
        creator->connect(cb, SIGNAL(clicked(bool)), creator, SLOT(optional_other_item_selected()));
        try { hlayouts.at(count++/max_per_row)->addWidget(cb); }
        catch (const std::out_of_range& e) {
            QMessageBox message_box;
            message_box.critical(nullptr, creator->tr("Error"), creator->tr(e.what()));
            message_box.setFixedSize(500, 200);
        }
    }
    for (auto l : hlayouts) l->addStretch(1);
    for (auto i = 0U; i < frames.size(); ++i) {
        frames[i]->setLayout(hlayouts[i]);
        vlayout->addWidget(frames[i]);
    }
    box->setLayout(vlayout);
    return box;
}

bool MagicItemBox::reinitialise(ItemType focus) {
    std::shared_ptr<
        std::pair<
            std::string,
            std::unordered_map<std::string, item>
        >
    > mih;
    if (current == nullptr) return false;
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
    default: return false;
    }
    auto weapons_tab = make_items_tab(mih->second, ItemType::WEAPON);
    if (weapons_tab != nullptr) box->addTab(weapons_tab, creator->tr("Magic Weapons"));
    // magic armour
    auto armour_tab = make_items_tab(mih->second, ItemType::ARMOUR);
    if (armour_tab != nullptr) box->addTab(armour_tab, creator->tr("Magic Armour"));
    // talismans
    auto talismans_tab = make_items_tab(mih->second, ItemType::TALISMAN);
    if (talismans_tab != nullptr) box->addTab(talismans_tab, creator->tr("Talismans"));
    // enchanted items
    auto enchanted_tab = make_items_tab(mih->second, ItemType::ENCHANTED);
    if (enchanted_tab != nullptr) box->addTab(enchanted_tab, creator->tr("Enchanted Items"));
    // other items
    auto other_tab = make_items_tab(mih->second, ItemType::OTHER);
    if (other_tab != nullptr) box->addTab(other_tab, creator->tr("Other"));
    // arcane items
    QGroupBox* arcane_tab = nullptr;
    if (current->is_mage()) {
        arcane_tab = make_items_tab(mih->second, ItemType::ARCANE);
        if (arcane_tab != nullptr) box->addTab(arcane_tab, creator->tr("Arcane Items"));
    }
    QGroupBox* banner_tab = nullptr;
    switch (current->base_unit_type()) {
    case BaseUnitType::MAGE_CHARACTER:
    case BaseUnitType::MELEE_CHARACTER:
    {
        if (std::dynamic_pointer_cast<character_unit>(current)->is_bsb()) {
            banner_tab = make_items_tab(mih->second, ItemType::BANNER);
            if (banner_tab != nullptr) box->addTab(banner_tab, creator->tr("Magic Standards"));
        }
        break;
    }
    case BaseUnitType::NORMAL:
    {
        if (std::dynamic_pointer_cast<normal_unit>(current)->handle->magic_banner_budget() > 0.0) {
            banner_tab = make_items_tab(mih->second, ItemType::BANNER);
            if (banner_tab != nullptr) box->addTab(banner_tab, creator->tr("Magic Standards"));
        }
        break;
    }
    default: break;
    }
    switch (focus) {
    case ItemType::WEAPON:
        if (weapons_tab != nullptr)
            box->setCurrentWidget(weapons_tab);
        break;
    case ItemType::ARMOUR:
        if (armour_tab != nullptr)
            box->setCurrentWidget(armour_tab);
        break;
    case ItemType::TALISMAN:
        if (talismans_tab != nullptr)
            box->setCurrentWidget(talismans_tab);
        break;
    case ItemType::ARCANE:
        if (arcane_tab != nullptr)
            box->setCurrentWidget(arcane_tab);
        break;
    case ItemType::ENCHANTED:
        if (enchanted_tab != nullptr)
            box->setCurrentWidget(enchanted_tab);
        break;
    case ItemType::BANNER:
        if (banner_tab != nullptr)
            box->setCurrentWidget(banner_tab);
        break;
    case ItemType::OTHER:
        if (other_tab != nullptr)
            box->setCurrentWidget(other_tab);
        break;
    }
    return true;
}
