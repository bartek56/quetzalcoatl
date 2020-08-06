#include "artistsitem.h"
#include "artistitem.h"

ArtistsItem::ArtistsItem(Item *parent)
    : Item(QIcon::fromTheme("server-database"), Qt::ItemIsEnabled, true, true, parent)
{}

QString ArtistsItem::text(int column) const
{
    if (0 == column) {
        return "Artists";
    }
    return QString();
}

QVector<Item *> ArtistsItem::fetchMore(Controller *controller)
{
    QVector<Item *> items;
    for (auto artist : controller->listTags(MPD_TAG_ARTIST)) {
        items.append(new ArtistItem(artist));
    }
    return items;
}
