#ifndef MAGIC_ITEM_WINDOW_H
#define MAGIC_ITEM_WINDOW_H

#include "army_maps.h"
#include "magic_item.h"

#include <memory>
#include <unordered_map>

#include <QTableWidget>
#include <QWidget>

class MagicItemWindow : public QWidget {
    Q_OBJECT

public:
    explicit MagicItemWindow(ItemType item_type_);
    ~MagicItemWindow();

private:
    ItemType item_type;
};


#endif // !MAGIC_ITEM_WINDOW_H
