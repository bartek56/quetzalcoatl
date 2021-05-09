#ifndef connection_h
#define connection_h

#include "entity.h"
#include "playlist.h"
#include "song.h"
#include "status.h"
#include <memory>
#include <mpd/client.h>
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

    virtual mpd_error connection_get_error();
    virtual const char *connection_get_error_message();
    virtual const unsigned *connection_get_server_version();
    virtual bool clear_error();
    virtual std::vector<std::unique_ptr<entity>> list_queue_meta();
    virtual std::vector<std::unique_ptr<entity>> list_playlist_meta(const char *);
    virtual std::vector<std::unique_ptr<entity>> list_all_meta(const char *);
    virtual int connection_get_fd();
    virtual bool send_idle();
    virtual mpd_idle recv_idle(bool);
    virtual std::unique_ptr<mpdclient::status> run_status();
    virtual std::vector<plchangeposid> queue_changes_brief(unsigned);
    std::unique_ptr<mpdclient::song> run_get_queue_song_id(unsigned);
    virtual mpd_idle run_noidle();
    virtual bool search_db_songs(bool);
    virtual bool search_add_tag_constraint(mpd_operator, mpd_tag_type, const char *);
    virtual std::vector<std::unique_ptr<song>> search_commit();
    virtual bool command_list_begin(bool);
    virtual bool command_list_end();
    virtual bool send_move_id(unsigned, unsigned);
    virtual std::vector<std::unique_ptr<mpdclient::playlist>> list_playlists();

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