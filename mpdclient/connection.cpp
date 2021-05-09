#include "mpdclient/connection.h"

mpd::connection::connection(mpd_connection *connection)
    : m_connection{connection}
{}

mpd::connection::operator bool() const
{
    return bool(m_connection);
}

mpd_error mpd::connection::get_error()
{
    return mpd_connection_get_error(m_connection);
};

const char *mpd::connection::get_error_message()
{
    return mpd_connection_get_error_message(m_connection);
}

const unsigned *mpd::connection::get_server_version()
{
    return mpd_connection_get_server_version(m_connection);
}

bool mpd::connection::clear_error()
{
    return mpd_connection_clear_error(m_connection);
}

std::vector<std::unique_ptr<mpd::song>> mpd::connection::list_queue_meta()
{
    // Precondition: m_connection is not null

    std::vector<std::unique_ptr<mpd::song>> songs;
    if (mpd_send_list_queue_meta(m_connection)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(
                    std::make_unique<mpd::song>(mpd_song_dup(mpd_entity_get_song(entity))));
            }
        }
    }
    return songs;

    // Postcondition: getError() will have the success or failure status. The vector will be
    // empty if there was a problem.
}

int mpd::connection::get_fd()
{
    return mpd_connection_get_fd(m_connection);
}

bool mpd::connection::send_idle()
{
    return mpd_send_idle(m_connection);
}

mpd_idle mpd::connection::recv_idle(bool disable_timeout)
{
    return mpd_recv_idle(m_connection, disable_timeout);
}

std::unique_ptr<mpd::status> mpd::connection::run_status()
{
    return std::make_unique<mpd::status>(mpd_run_status(m_connection));
}

std::vector<mpd::plchangeposid> mpd::connection::queue_changes_brief(unsigned version)
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

std::unique_ptr<mpd::song> mpd::connection::run_get_queue_song_id(unsigned id)
{
    return std::make_unique<mpd::song>(mpd_run_get_queue_song_id(m_connection, id));
}

mpd_idle mpd::connection::run_noidle()
{
    return mpd_run_noidle(m_connection);
}

bool mpd::connection::search_db_songs(bool exact)
{
    return mpd_search_db_songs(m_connection, exact);
}

bool mpd::connection::search_add_tag_constraint(mpd_operator oper,
                                                mpd_tag_type type,
                                                const char *value)
{
    return mpd_search_add_tag_constraint(m_connection, oper, type, value);
}

std::vector<std::unique_ptr<mpd::song>> mpd::connection::search_commit()
{
    std::vector<std::unique_ptr<mpd::song>> songs;

    if (mpd_search_commit(m_connection)) {
        while (mpd_song *song_ptr = mpd_recv_song(m_connection)) {
            songs.push_back(std::make_unique<mpd::song>(song_ptr));
        }
    }
    return songs;
}

bool mpd::connection::command_list_begin(bool discrete_ok)
{
    return mpd_command_list_begin(m_connection, discrete_ok);
}

bool mpd::connection::command_list_end()
{
    return mpd_command_list_end(m_connection);
}

bool mpd::connection::send_move_id(unsigned from, unsigned to)
{
    return mpd_send_move_id(m_connection, from, to);
}

mpd::connection::connection(mpd::connection &&other)
    : m_connection(other.m_connection)
{
    other.m_connection = nullptr;
}

mpd::connection &mpd::connection::operator=(mpd::connection &&other)
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

mpd::connection::~connection()
{
    if (m_connection) {
        mpd_connection_free(m_connection);
    }
}
