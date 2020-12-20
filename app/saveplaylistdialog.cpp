#include "saveplaylistdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

SavePlaylistDialog::SavePlaylistDialog(QValidator *validator, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    setWindowTitle("Save Playlist");
    auto layout = new QVBoxLayout();
    auto formLayout = new QFormLayout();
    m_nameEdit = new QLineEdit();
    m_nameEdit->setValidator(validator);
    formLayout->addRow("&Name", m_nameEdit);
    layout->addLayout(formLayout);
    auto buttonBox = new QDialogButtonBox();
    auto okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(buttonBox);
    auto cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    layout->addWidget(buttonBox);
    setLayout(layout);
}

QString SavePlaylistDialog::name() const
{
    return m_nameEdit->text().trimmed();
}

void SavePlaylistDialog::clear()
{
    m_nameEdit->clear();
}