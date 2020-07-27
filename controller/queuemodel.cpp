#include "queuemodel.h"
#include "item.h"
#include <QByteArray>
#include <QDebug>
#include <QFont>

QueueModel::QueueModel(Controller *controller, QObject *parent)
    : ItemModel(controller, parent)
    , m_songId(-1)
{
    setRootItem(new Item(QIcon(), Qt::NoItemFlags, true, false));

    connect(controller, &Controller::queueChanged, this, &QueueModel::setQueue);
    connect(controller, &Controller::songId, this, &QueueModel::setSongId);
}

QVariant QueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QStringLiteral("Name");

        case 1:
            return QStringLiteral("Time");

        default:
            return QVariant();
        }
    }
    return QVariant();
}

int QueueModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

Qt::ItemFlags QueueModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        if (index.column() == 0) {
            return static_cast<Item *>(index.internalPointer())->flags();
        }

        return Qt::NoItemFlags;
    }

    // The drop target is between rows.
    return Qt::ItemIsDropEnabled;
}

QStringList QueueModel::mimeTypes() const
{
    return QStringList{"x-application/vnd.mpd.songids", "x-application/vnd.mpd.uris"};
}

QMimeData *QueueModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    for (auto &index : indexes) {
        stream << static_cast<unsigned>(index.row());
        auto songItem = static_cast<Item *>(index.internalPointer());
        stream << songItem->id();
    }
    mimeData->setData("x-application/vnd.mpd.songids", encodedData);
    return mimeData;
}

bool QueueModel::canDropMimeData(const QMimeData *data,
                                 Qt::DropAction action,
                                 int row,
                                 int column,
                                 const QModelIndex &parent) const
{
    Q_UNUSED(action)
    Q_UNUSED(parent)

    if (row < 0) {
        return false;
    }

    if (!data->hasFormat("x-application/vnd.mpd.songids")
        && !data->hasFormat("x-application/vnd.mpd.uris")) {
        return false;
    }

    if (column > 0) {
        return false;
    }

    return true;
}

bool QueueModel::dropMimeData(
    const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action)
    Q_UNUSED(column)
    Q_UNUSED(parent)

    if (!controller()) {
        return false;
    }

    // source row, song id
    QVector<QPair<unsigned, unsigned>> sources;

    if (data->hasFormat("x-application/vnd.mpd.songids")) {
        QByteArray encodedData = data->data("x-application/vnd.mpd.songids");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QVector<unsigned> songs;

        while (!stream.atEnd()) {
            QPair<int, unsigned> source;
            stream >> source.first;
            stream >> source.second;
            sources.append(source);
        }

        controller()->moveSongs(sources, row);
    } else if (data->hasFormat("x-application/vnd.mpd.uris")) {
        qDebug() << "Handling the dropping of URIs at " << row;
        QByteArray encodedData = data->data("x-application/vnd.mpd.uris");
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        QVector<QString> uris;
        QString uri;
        while (!stream.atEnd()) {
            stream >> uri;
            uris.append(uri);
        }

        controller()->queueUris(uris, row);
    }

    return true;
}

QVariant QueueModel::data(const QModelIndex &index, int role) const
{
    if (Qt::FontRole == role) {
        if (m_songId != -1) {
            unsigned playingId = static_cast<unsigned>(m_songId);
            auto rowId = static_cast<Item *>(index.internalPointer())->id();
            if (rowId == playingId) {
                QFont font;
                font.setBold(true);
                return font;
            }
        }
    }

    return ItemModel::data(index, role);
}

void QueueModel::setQueue(const QVector<Item *> &queue)
{
    beginResetModel();
    rootItem()->clear();
    for (auto songItem : queue) {
        rootItem()->append(songItem);
    }
    endResetModel();
    emit columnResized(0);
    emit columnResized(1);
}

void QueueModel::setSongId(int songId)
{
    if (m_songId == songId) {
        return;
    }

    int oldSongId = m_songId;
    m_songId = songId;

    for (int i = 0; i < rootItem()->children().count(); i++) {
        unsigned id = static_cast<Item *>(rootItem()->children().at(i))->id();

        // The previously playing song is no longer bolded.
        if (oldSongId > 0 && id == static_cast<unsigned>(oldSongId)) {
            emit dataChanged(index(i, 0, QModelIndex()), index(i, 0, QModelIndex()), {Qt::FontRole});
        }

        // The currently playing song is now bolded.
        if (m_songId > 0 && id == static_cast<unsigned>(m_songId)) {
            emit dataChanged(index(i, 0, QModelIndex()), index(i, 0, QModelIndex()), {Qt::FontRole});
        }
    }
}
