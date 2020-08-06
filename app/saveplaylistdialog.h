#ifndef SAVEPLAYLISTDIALOG_H
#define SAVEPLAYLISTDIALOG_H

#include "controller.h"
#include <QDialog>
#include <QLineEdit>

class SavePlaylistDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SavePlaylistDialog(QValidator *,
                                QWidget *parent = nullptr,
                                Qt::WindowFlags f = Qt::WindowFlags());

    QString name() const;
signals:
private:
    QLineEdit *m_nameEdit;
};

#endif // SAVEPLAYLISTDIALOG_H
