#include "mpdclient/song.h"

const char *mpdclient::song::get_tag(mpd_tag_type type, unsigned idx) noexcept
{
    return mpd_song_get_tag(m_song, type, idx);
}

mpdclient::song::operator bool() const
{
    return m_song;
}

mpdclient::song::song(mpd_song *song)
    : m_song(song)
{}

mpdclient::song::song(mpdclient::song &other)
{
    m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
}

mpdclient::song::song(mpdclient::song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
}

mpdclient::song &mpdclient::song::operator=(const mpdclient::song &other)
{
    if (this != &other) {
        m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
    }
    return *this;
}

mpdclient::song &mpdclient::song::operator=(mpdclient::song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
    return *this;
}

mpdclient::song::~song()
{
    if (m_song) {
        mpd_song_free(m_song);
    }
}