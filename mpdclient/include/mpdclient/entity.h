#ifndef ENTITY_H
#define ENTITY_H

#include "directory.h"
#include "playlist.h"
#include "song.h"
#include <memory>
#include <mpd/client.h>

namespace mpdclient {
class entity
{
public:
    entity(mpd_entity * = nullptr);
    virtual explicit operator bool() const;
    mpd_entity_type get_type();
    std::unique_ptr<mpdclient::directory> get_directory();
    std::unique_ptr<mpdclient::song> get_song();
    std::unique_ptr<mpdclient::playlist> get_playlist();

    entity(const entity &) = delete;
    entity &operator=(const entity &) = delete;
    entity(entity &&);
    entity &operator=(entity &&);
    virtual ~entity();

private:
    mpd_entity *m_entity{};
};
}; // namespace mpdclient

#endif