#include "playlistdelegate.h"
#include "item.h"
#include <QDebug>
#include <QLineEdit>

PlaylistDelegate::PlaylistDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

QWidget *PlaylistDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    return new QLineEdit(parent);
}

void PlaylistDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    auto textEdit = static_cast<QLineEdit *>(editor);
    auto item = static_cast<Item *>(index.internalPointer());
    textEdit->setText(item->text(0));
}

void PlaylistDelegate::setModelData(QWidget *editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    qDebug() << "Calling setModelData";
}
