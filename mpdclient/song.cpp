#include "mpdclient/song.h"

const char *mpd::song::get_tag(mpd_tag_type type, unsigned idx)
{
    return mpd_song_get_tag(m_song, type, idx);
}

mpd::song::operator bool() const
{
    return m_song;
}

mpd::song::song(mpd_song *song)
    : m_song(song)
{}

mpd::song::song(mpd::song &other)
{
    m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
}

mpd::song::song(mpd::song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
}

mpd::song &mpd::song::operator=(const mpd::song &other)
{
    if (this != &other) {
        m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
    }
    return *this;
}

mpd::song &mpd::song::operator=(mpd::song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
    return *this;
}

mpd::song::~song()
{
    if (m_song) {
        mpd_song_free(m_song);
    }
}