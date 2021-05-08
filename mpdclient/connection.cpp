#include "mpdclient/connection.h"

mpd::Connection::Connection(mpd_connection *connection)
    : m_connection{connection}
{}

mpd::Connection::operator bool()
{
    return bool(m_connection);
}

mpd_error mpd::Connection::get_error()
{
    return mpd_connection_get_error(m_connection);
};

const char *mpd::Connection::get_error_message()
{
    return mpd_connection_get_error_message(m_connection);
}

const unsigned *mpd::Connection::get_server_version()
{
    return mpd_connection_get_server_version(m_connection);
}

bool mpd::Connection::clearError()
{
    return mpd_connection_clear_error(m_connection);
}

std::vector<std::unique_ptr<mpd::Song>> mpd::Connection::list_queue_meta()
{
    // Precondition: m_connection is not null

    std::vector<std::unique_ptr<mpd::Song>> songs;
    if (mpd_send_list_queue_meta(m_connection)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(
                    std::make_unique<mpd::Song>(mpd_song_dup(mpd_entity_get_song(entity))));
            }
        }
    }
    return songs;

    // Postcondition: getError() will have the success or failure status. The vector will be
    // empty if there was a problem.
}

int mpd::Connection::get_fd()
{
    return mpd_connection_get_fd(m_connection);
}

bool mpd::Connection::send_idle()
{
    return mpd_send_idle(m_connection);
}

mpd_idle mpd::Connection::recv_idle(bool disable_timeout)
{
    return mpd_recv_idle(m_connection, disable_timeout);
}

std::unique_ptr<mpd::Status> mpd::Connection::status()
{
    return std::make_unique<mpd::Status>(mpd_run_status(m_connection));
}

std::vector<mpd::plchangeposid> mpd::Connection::plchangesposid(unsigned version)
{
    std::vector<mpd::plchangeposid> changes;
    if (mpd_send_queue_changes_brief(m_connection, version)) {
        struct plchangeposid change;
        while (mpd_recv_queue_change_brief(m_connection, &change.position, &change.id)) {
            changes.push_back(change);
        }
    }

    return changes;
}

std::unique_ptr<mpd::Song> mpd::Connection::get_queue_song_id(unsigned id)
{
    return std::make_unique<mpd::Song>(mpd_run_get_queue_song_id(m_connection, id));
}

mpd_idle mpd::Connection::noidle()
{
    return mpd_run_noidle(m_connection);
}

bool mpd::Connection::search_db_songs(bool exact)
{
    return mpd_search_db_songs(m_connection, exact);
}

bool mpd::Connection::search_add_tag_constraint(mpd_operator oper,
                                                mpd_tag_type type,
                                                const char *value)
{
    return mpd_search_add_tag_constraint(m_connection, oper, type, value);
}

std::vector<std::unique_ptr<mpd::Song>> mpd::Connection::search_commit()
{
    std::vector<std::unique_ptr<mpd::Song>> songs;

    if (mpd_search_commit(m_connection)) {
        while (mpd_song *song_ptr = mpd_recv_song(m_connection)) {
            songs.push_back(std::make_unique<mpd::Song>(song_ptr));
        }
    }
    return songs;
}

mpd::Connection::Connection(mpd::Connection &&other)
    : m_connection(other.m_connection)
{
    other.m_connection = nullptr;
}

mpd::Connection &mpd::Connection::operator=(mpd::Connection &&other)
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

mpd::Connection::~Connection()
{
    if (m_connection) {
        mpd_connection_free(m_connection);
    }
}
