#include "mpdclient/entity.h"

mpdclient::entity::entity(mpd_entity *entity_ptr)
    : m_entity(entity_ptr)
{}

mpdclient::entity::operator bool() const
{
    return m_entity;
}

mpd_entity_type mpdclient::entity::get_type()
{
    return mpd_entity_get_type(m_entity);
}

std::unique_ptr<mpdclient::song> mpdclient::entity::get_song()
{
    return std::make_unique<mpdclient::song>(mpd_song_dup(mpd_entity_get_song(m_entity)));
}

std::unique_ptr<mpdclient::playlist> mpdclient::entity::get_playlist()
{
    return std::make_unique<mpdclient::playlist>(
        mpd_playlist_dup(mpd_entity_get_playlist(m_entity)));
}

mpdclient::entity::entity(mpdclient::entity &&other)
    : m_entity(other.m_entity)
{
    other.m_entity = nullptr;
}

mpdclient::entity &mpdclient::entity::operator=(mpdclient::entity &&other)
{
    if (this != &other) {
        if (m_entity) {
            mpd_entity_free(m_entity);
        }

        m_entity = other.m_entity;
        other.m_entity = nullptr;
    }
    return *this;
}

mpdclient::entity::~entity()
{
    if (m_entity) {
        mpd_entity_free(m_entity);
    }
}