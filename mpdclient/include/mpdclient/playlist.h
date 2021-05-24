#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <mpd/client.h>

namespace mpdclient {
class playlist
{
public:
    playlist(mpd_playlist * = nullptr);
    virtual explicit operator bool() const;

    const char *get_path() noexcept;

    playlist(playlist &);
    playlist(playlist &&);
    playlist &operator=(const playlist &);
    playlist &operator=(playlist &&);
    virtual ~playlist();

private:
    mpd_playlist *m_playlist{};
};
}; // namespace mpdclient

#endif