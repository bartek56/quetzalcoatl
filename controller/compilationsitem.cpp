#include "compilationsitem.h"
#include "compilationartistitem.h"
#include "controller.h"
#include "iconnames.h"

CompilationsItem::CompilationsItem(Item *parent)
    : Item(QIcon::fromTheme(IconNames::Database), Qt::ItemIsEnabled, true, true, parent)
{}

QString CompilationsItem::text(int column) const
{
    if (0 == column) {
        return "Compilations";
    }
    return QString();
}

QVector<Item *> CompilationsItem::fetchMore(Controller *controller)
{
    QVector<Item *> items;
    for (auto albumArtist : controller->listTags(MPD_TAG_ALBUM_ARTIST)) {
        items.append(new CompilationArtistItem(albumArtist));
    }
    return items;
}
