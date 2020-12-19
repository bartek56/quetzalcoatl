#include "playlistdelegate.h"
#include "databasemodel.h"
#include <QLineEdit>

PlaylistDelegate::PlaylistDelegate(QValidator *validator, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_validator(validator)
{}

QWidget *PlaylistDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const
{
    auto lineEdit = new QLineEdit(parent);
    lineEdit->setValidator(m_validator);

    // Plus dot, plus extension, makes 255.
    lineEdit->setMaxLength(251);
    return lineEdit;
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
    auto lineEdit = static_cast<QLineEdit *>(editor);
    auto dbModel = static_cast<DatabaseModel *>(model);
    auto name = lineEdit->text().trimmed();
    if (!name.isEmpty()) {
        dbModel->renamePlaylist(index, name);
    }
}
