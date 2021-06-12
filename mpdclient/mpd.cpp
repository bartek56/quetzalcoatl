#include "mpdclient/mpd.h"
#include <string>

mpdclient::mpd::mpd(mpd_connection *connection)
    : m_connection{connection}
{}

mpdclient::mpd::operator bool() const
{
    return bool(m_connection);
}

mpd_error mpdclient::mpd::connection_get_error() noexcept
{
    return mpd_connection_get_error(m_connection);
};

const char *mpdclient::mpd::connection_get_error_message() noexcept
{
    return mpd_connection_get_error_message(m_connection);
}

const unsigned *mpdclient::mpd::connection_get_server_version() noexcept
{
    return mpd_connection_get_server_version(m_connection);
}

bool mpdclient::mpd::clear_error() noexcept
{
    return mpd_connection_clear_error(m_connection);
}

bool mpdclient::mpd::send_clear() noexcept
{
    return mpd_send_clear(m_connection);
}

bool mpdclient::mpd::send_add(const char *file) noexcept
{
    return mpd_send_add(m_connection, file);
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::list_queue_meta() noexcept
{
    // Precondition: m_connection is not null

    std::vector<std::unique_ptr<mpdclient::song>> songs;
    if (mpd_send_list_queue_meta(m_connection)) {
        while (auto entity = mpdclient::entity(mpd_recv_entity(m_connection))) {
            if (entity.get_type() == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(entity.get_song());
            }
        }
    }
    return songs;

    // Postcondition: getError() will have the success or failure status. The vector will be
    // empty if there was a problem.
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::list_playlist_meta(
    const char *name) noexcept
{
    std::vector<std::unique_ptr<mpdclient::song>> songs;
    if (mpd_send_list_playlist_meta(m_connection, name)) {
        while (auto entity = mpdclient::entity(mpd_recv_entity(m_connection))) {
            if (entity.get_type() == MPD_ENTITY_TYPE_SONG) {
                songs.push_back(entity.get_song());
            }
        }
    }
    return songs;
}

std::vector<std::unique_ptr<mpdclient::entity>> mpdclient::mpd::list_all_meta(
    const char *path) noexcept
{
    std::vector<std::unique_ptr<mpdclient::entity>> entities;
    if (mpd_send_list_all_meta(m_connection, path)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            entities.push_back(std::make_unique<mpdclient::entity>(entity));
        }
    }
    return entities;
}

std::vector<std::unique_ptr<mpdclient::entity>> mpdclient::mpd::list_meta(const char *path) noexcept
{
    std::vector<std::unique_ptr<mpdclient::entity>> entities;
    if (mpd_send_list_meta(m_connection, path)) {
        while (auto entity = mpd_recv_entity(m_connection)) {
            entities.push_back(std::make_unique<mpdclient::entity>(entity));
        }
    }
    return entities;
}

int mpdclient::mpd::connection_get_fd() noexcept
{
    return mpd_connection_get_fd(m_connection);
}

bool mpdclient::mpd::send_idle() noexcept
{
    return mpd_send_idle(m_connection);
}

mpd_idle mpdclient::mpd::recv_idle(bool disable_timeout) noexcept
{
    return mpd_recv_idle(m_connection, disable_timeout);
}

std::unique_ptr<mpdclient::status> mpdclient::mpd::run_status() noexcept
{
    return std::make_unique<mpdclient::status>(mpd_run_status(m_connection));
}

std::vector<mpdclient::plchangeposid> mpdclient::mpd::queue_changes_brief(unsigned version) noexcept
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

std::unique_ptr<mpdclient::song> mpdclient::mpd::run_get_queue_song_id(unsigned id) noexcept
{
    return std::make_unique<mpdclient::song>(mpd_run_get_queue_song_id(m_connection, id));
}

mpd_idle mpdclient::mpd::run_noidle() noexcept
{
    return mpd_run_noidle(m_connection);
}

bool mpdclient::mpd::search_db_songs(bool exact) noexcept
{
    return mpd_search_db_songs(m_connection, exact);
}

bool mpdclient::mpd::search_add_tag_constraint(mpd_operator oper,
                                               mpd_tag_type type,
                                               const char *value) noexcept
{
    return mpd_search_add_tag_constraint(m_connection, oper, type, value);
}

std::vector<std::unique_ptr<mpdclient::song>> mpdclient::mpd::recv_songs() noexcept
{
    std::vector<std::unique_ptr<mpdclient::song>> songs;

    while (mpd_song *song_ptr = mpd_recv_song(m_connection)) {
        songs.push_back(std::make_unique<mpdclient::song>(song_ptr));
    }
    return songs;
}

bool mpdclient::mpd::search_commit() noexcept
{
    return mpd_search_commit(m_connection);
}

bool mpdclient::mpd::search_db_tags(mpd_tag_type type) noexcept
{
    return mpd_search_db_tags(m_connection, type);
}

bool mpdclient::mpd::command_list_begin(bool discrete_ok) noexcept
{
    return mpd_command_list_begin(m_connection, discrete_ok);
}

bool mpdclient::mpd::command_list_end() noexcept
{
    return mpd_command_list_end(m_connection);
}

bool mpdclient::mpd::send_move_id(unsigned from, unsigned to) noexcept
{
    return mpd_send_move_id(m_connection, from, to);
}

std::vector<std::unique_ptr<mpdclient::playlist>> mpdclient::mpd::list_playlists() noexcept
{
    std::vector<std::unique_ptr<mpdclient::playlist>> playlists;
    if (mpd_send_list_playlists(m_connection)) {
        while (mpd_playlist *playlist = mpd_recv_playlist(m_connection)) {
            playlists.push_back(std::make_unique<mpdclient::playlist>(playlist));
        }
    }
    return playlists;
}

bool mpdclient::mpd::send_add_id_to(const char *uri, unsigned to) noexcept
{
    return mpd_send_add_id_to(m_connection, uri, to);
}

bool mpdclient::mpd::response_finish() noexcept
{
    return mpd_response_finish(m_connection);
}

std::vector<std::string> mpdclient::mpd::recv_tags(mpd_tag_type type) noexcept
{
    std::vector<std::string> tags;
    while (mpd_pair *pair = mpd_recv_pair_tag(m_connection, type)) {
        // There is no use case where I will ever find pair->name useful.
        tags.push_back(std::string(pair->value));
        mpd_return_pair(m_connection, pair);
    }
    return tags;
}

bool mpdclient::mpd::run_play_id(unsigned song_id) noexcept
{
    return mpd_run_play_id(m_connection, song_id);
}

int mpdclient::mpd::run_add_id(const char *file) noexcept
{
    return mpd_run_add_id(m_connection, file);
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
