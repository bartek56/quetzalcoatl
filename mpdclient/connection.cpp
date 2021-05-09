#include "mpdclient/connection.h"

mpdclient::mpd::mpd(mpd_connection *connection)
    : m_connection{connection}
{}

mpdclient::mpd::operator bool() const
{
    return bool(m_connection);
}

mpd_error mpdclient::mpd::connection_get_error()
{
    return mpd_connection_get_error(m_connection);
};

const char *mpdclient::mpd::connection_get_error_message()
{
    return mpd_connection_get_error_message(m_connection);
}

const unsigned *mpdclient::mpd::connection_get_server_version()
{
    return mpd_connection_get_server_version(m_connection);
}

bool mpdclient::mpd::clear_error()
{
    return mpd_connection_clear_error(m_connection);
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::list_queue_meta()
{
    // Precondition: m_connection is not null

    std::vector<std::unique_ptr<mpdclient::song>> songs;
    if (mpd_send_list_queue_meta(m_connection)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(
                    std::make_unique<mpdclient::song>(mpd_song_dup(mpd_entity_get_song(entity))));
            }
            mpd_entity_free(entity);
        }
    }
    return songs;

    // Postcondition: getError() will have the success or failure status. The vector will be
    // empty if there was a problem.
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::list_playlist_meta(const char *name)
{
    std::vector<std::unique_ptr<mpdclient::song>> songs;
    if (mpd_send_list_playlist_meta(m_connection, name)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(
                    std::make_unique<mpdclient::song>(mpd_song_dup(mpd_entity_get_song(entity))));
            }
            mpd_entity_free(entity);
        }
    }
    return songs;
}

int mpdclient::mpd::connection_get_fd()
{
    return mpd_connection_get_fd(m_connection);
}

bool mpdclient::mpd::send_idle()
{
    return mpd_send_idle(m_connection);
}

mpd_idle mpdclient::mpd::recv_idle(bool disable_timeout)
{
    return mpd_recv_idle(m_connection, disable_timeout);
}

std::unique_ptr<mpdclient::status> mpdclient::mpd::run_status()
{
    return std::make_unique<mpdclient::status>(mpd_run_status(m_connection));
}

std::vector<mpdclient::plchangeposid> mpdclient::mpd::queue_changes_brief(unsigned version)
{
    std::vector<mpdclient::plchangeposid> changes;
    if (mpd_send_queue_changes_brief(m_connection, version)) {
        struct plchangeposid change;
        while (mpd_recv_queue_change_brief(m_connection, &change.position, &change.id)) {
            changes.push_back(change);
        }
    }

    return changes;
}

std::unique_ptr<mpdclient::song> mpdclient::mpd::run_get_queue_song_id(unsigned id)
{
    return std::make_unique<mpdclient::song>(mpd_run_get_queue_song_id(m_connection, id));
}

mpd_idle mpdclient::mpd::run_noidle()
{
    return mpd_run_noidle(m_connection);
}

bool mpdclient::mpd::search_db_songs(bool exact)
{
    return mpd_search_db_songs(m_connection, exact);
}

bool mpdclient::mpd::search_add_tag_constraint(mpd_operator oper,
                                               mpd_tag_type type,
                                               const char *value)
{
    return mpd_search_add_tag_constraint(m_connection, oper, type, value);
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::search_commit()
{
    std::vector<std::unique_ptr<mpdclient::song>> songs;

    if (mpd_search_commit(m_connection)) {
        while (mpd_song *song_ptr = mpd_recv_song(m_connection)) {
            songs.push_back(std::make_unique<mpdclient::song>(song_ptr));
        }
    }
    return songs;
}

bool mpdclient::mpd::command_list_begin(bool discrete_ok)
{
    return mpd_command_list_begin(m_connection, discrete_ok);
}

bool mpdclient::mpd::command_list_end()
{
    return mpd_command_list_end(m_connection);
}

bool mpdclient::mpd::send_move_id(unsigned from, unsigned to)
{
    return mpd_send_move_id(m_connection, from, to);
}

std::vector<std::unique_ptr<mpdclient::playlist>> mpdclient::mpd::list_playlists()
{
    std::vector<std::unique_ptr<mpdclient::playlist>> playlists;
    if (mpd_send_list_playlists(m_connection)) {
        while (mpd_playlist *playlist = mpd_recv_playlist(m_connection)) {
            playlists.push_back(std::make_unique<mpdclient::playlist>(playlist));
        }
    }
    return playlists;
}

mpdclient::mpd::mpd(mpdclient::mpd &&other)
    : m_connection(other.m_connection)
{
    other.m_connection = nullptr;
}

mpdclient::mpd &mpdclient::mpd::operator=(mpdclient::mpd &&other)
{
    if (this != &other) {
        if (m_connection) {
            mpd_connection_free(m_connection);
        }

        m_connection = other.m_connection;
        other.m_connection = nullptr;
    }
    return *this;
}

mpdclient::mpd::~mpd()
{
    if (m_connection) {
        mpd_connection_free(m_connection);
    }
}
