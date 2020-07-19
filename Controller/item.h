#ifndef ITEM_H
#define ITEM_H

#include <QVariant>
#include <QVector>
#include <QtGui/QIcon>

class Item
{
public:
    explicit Item(QIcon, Qt::ItemFlags, bool, Item *parent = nullptr);
    virtual ~Item();

    void append(Item *at);

    Item *at(int row);
    int count() const;
    QIcon icon() const;
    virtual QString text(int) const;
    int row() const;
    Item *parent();
    void setParent(Item *parent);

    void clear();

    virtual QVariant tooltip();

    Qt::ItemFlags flags() const;

    virtual bool canFetchMore();
    virtual void fetchMore();
    virtual bool hasChildren();

private:
    QVector<Item *> m_children;
    QIcon m_icon;
    Item *m_parent;
    Qt::ItemFlags m_flags;
    bool m_hasChildren;
};

#endif // ITEM_H
