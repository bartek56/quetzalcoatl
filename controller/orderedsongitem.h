#ifndef ORDEREDSONGITEM_H
#define ORDEREDSONGITEM_H

#include "songitem.h"

class OrderedSongItem : public SongItem
{
public:
    explicit OrderedSongItem(QIcon, Qt::ItemFlags, bool, bool, Item *parent = nullptr);
    void onDoubleClicked(Controller *) override;
};

#endif // ORDEREDSONGITEM_H
