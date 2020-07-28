#ifndef QUEUEDITEM_H
#define QUEUEDITEM_H

#include "abstractsongitem.h"
#include <mpd/client.h>

class QueuedItem : public AbstractSongItem
{
public:
    QueuedItem(mpd_entity *, Item *parent = nullptr);
    ~QueuedItem();

    void onDoubleClicked(Controller *) override;
    unsigned id() override;

protected:
    const mpd_song *song() const override;

private:
    mpd_entity *m_entity;
};

#endif // QUEUEDITEM_H
