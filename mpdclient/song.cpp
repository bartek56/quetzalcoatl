#include "mpdclient/song.h"

const char *mpd::Song::get_tag(mpd_tag_type type, unsigned idx)
{
    return mpd_song_get_tag(m_song, type, idx);
}

mpd::Song::operator bool()
{
    return m_song;
}

mpd::Song::Song(mpd_song *song)
    : m_song(song)
{}

mpd::Song::Song(mpd::Song &other)
{
    m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
}

mpd::Song::Song(mpd::Song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
}

mpd::Song &mpd::Song::operator=(const mpd::Song &other)
{
    if (this != &other) {
        m_song = other.m_song ? mpd_song_dup(other.m_song) : other.m_song;
    }
    return *this;
}

mpd::Song &mpd::Song::operator=(mpd::Song &&other)
{
    m_song = other.m_song;
    other.m_song = nullptr;
    return *this;
}

mpd::Song::~Song()
{
    if (m_song) {
        mpd_song_free(m_song);
    }
}