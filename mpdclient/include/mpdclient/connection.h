#ifndef connection_h
#define connection_h

#include "song.h"
#include "status.h"
#include <memory>
#include <mpd/client.h>
#include <utility>
#include <vector>

namespace mpd {

struct plchangeposid
{
    unsigned position;
    unsigned id;
};

class connection
{
public:
    connection(mpd_connection * = nullptr);
    virtual explicit operator bool() const;

    virtual mpd_error get_error();
    virtual const char *get_error_message();
    virtual const unsigned *get_server_version();
    virtual bool clear_error();
    virtual std::vector<std::unique_ptr<song>> list_queue_meta();
    virtual int get_fd();
    virtual bool send_idle();
    virtual mpd_idle recv_idle(bool);
    virtual std::unique_ptr<mpd::status> run_status();
    virtual std::vector<plchangeposid> queue_changes_brief(unsigned);
    std::unique_ptr<mpd::song> run_get_queue_song_id(unsigned);
    virtual mpd_idle run_noidle();
    virtual bool search_db_songs(bool);
    virtual bool search_add_tag_constraint(mpd_operator, mpd_tag_type, const char *);
    virtual std::vector<std::unique_ptr<song>> search_commit();
    virtual bool command_list_begin(bool);
    virtual bool command_list_end();
    virtual bool send_move_id(unsigned, unsigned);

    connection(const connection &) = delete;
    connection &operator=(const connection &) = delete;
    connection(connection &&);
    connection &operator=(connection &&);
    virtual ~connection();

private:
    mpd_connection *m_connection{};
};

} // namespace mpd

#endif