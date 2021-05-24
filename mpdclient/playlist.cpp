#include "mpdclient/playlist.h"

const char *mpdclient::playlist::get_path() noexcept
{
    return mpd_playlist_get_path(m_playlist);
}

mpdclient::playlist::operator bool() const
{
    return m_playlist;
}

mpdclient::playlist::playlist(mpd_playlist *playlist)
    : m_playlist(playlist)
{}

mpdclient::playlist::playlist(mpdclient::playlist &other)
{
    m_playlist = other.m_playlist ? mpd_playlist_dup(other.m_playlist) : other.m_playlist;
}

mpdclient::playlist::playlist(mpdclient::playlist &&other)
{
    m_playlist = other.m_playlist;
    other.m_playlist = nullptr;
}

mpdclient::playlist &mpdclient::playlist::operator=(const mpdclient::playlist &other)
{
    if (this != &other) {
        m_playlist = other.m_playlist ? mpd_playlist_dup(other.m_playlist) : other.m_playlist;
    }
    return *this;
}

mpdclient::playlist &mpdclient::playlist::operator=(mpdclient::playlist &&other)
{
    m_playlist = other.m_playlist;
    other.m_playlist = nullptr;
    return *this;
}

mpdclient::playlist::~playlist()
{
    if (m_playlist) {
        mpd_playlist_free(m_playlist);
    }
}