#ifndef mpd_h
#define mpd_h

#include "entity.h"
#include "playlist.h"
#include "song.h"
#include "status.h"
#include <memory>
#include <mpd/client.h>
#include <string.h>
#include <utility>
#include <vector>

namespace mpdclient {

struct plchangeposid
{
    unsigned position;
    unsigned id;
};

class mpd
{
public:
    mpd(mpd_connection * = nullptr);
    virtual explicit operator bool() const;

    virtual mpd_error connection_get_error() noexcept;
    virtual const char *connection_get_error_message() noexcept;
    virtual const unsigned *connection_get_server_version() noexcept;
    virtual bool clear_error() noexcept;
    virtual std::vector<std::unique_ptr<song>> list_queue_meta() noexcept;
    virtual std::vector<std::unique_ptr<song>> list_playlist_meta(const char *) noexcept;
    virtual std::vector<std::unique_ptr<entity>> list_all_meta(const char *) noexcept;
    virtual std::vector<std::unique_ptr<entity>> list_meta(const char *) noexcept;
    virtual int connection_get_fd() noexcept;
    virtual bool send_idle() noexcept;
    virtual mpd_idle recv_idle(bool) noexcept;
    virtual std::unique_ptr<mpdclient::status> run_status() noexcept;
    virtual std::vector<plchangeposid> queue_changes_brief(unsigned) noexcept;
    std::unique_ptr<mpdclient::song> run_get_queue_song_id(unsigned) noexcept;
    virtual mpd_idle run_noidle() noexcept;
    virtual bool search_db_songs(bool) noexcept;
    virtual bool search_add_tag_constraint(mpd_operator, mpd_tag_type, const char *) noexcept;
    virtual bool search_commit() noexcept;
    virtual bool search_db_tags(mpd_tag_type) noexcept;
    virtual std::vector<std::unique_ptr<song>> recv_songs() noexcept;
    virtual bool command_list_begin(bool) noexcept;
    virtual bool command_list_end() noexcept;
    virtual bool send_move_id(unsigned, unsigned) noexcept;
    virtual std::vector<std::unique_ptr<mpdclient::playlist>> list_playlists() noexcept;
    virtual bool send_add_id_to(const char *, unsigned) noexcept;
    virtual bool response_finish() noexcept;
    virtual std::vector<std::string> recv_tags(mpd_tag_type) noexcept;
    virtual bool run_play_id(unsigned) noexcept;
    virtual int run_add_id(const char *) noexcept;
    virtual bool send_clear() noexcept;
    virtual bool send_add(const char *) noexcept;

    mpd(const mpd &) = delete;
    mpd &operator=(const mpd &) = delete;
    mpd(mpd &&);
    mpd &operator=(mpd &&);
    virtual ~mpd();

private:
    mpd_connection *m_connection{};
};

} // namespace mpdclient

#endif