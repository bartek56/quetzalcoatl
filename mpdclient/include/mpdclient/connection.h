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
    virtual explicit operator bool();

    virtual mpd_error get_error();
    virtual const char *get_error_message();
    virtual const unsigned *get_server_version();
    virtual bool clearError();
    virtual std::vector<std::unique_ptr<song>> list_queue_meta();
    virtual int get_fd();
    virtual bool send_idle();
    virtual mpd_idle recv_idle(bool);
    virtual std::unique_ptr<mpd::status> status();
    virtual std::vector<plchangeposid> plchangesposid(unsigned);
    std::unique_ptr<mpd::song> get_queue_song_id(unsigned);
    virtual mpd_idle noidle();
    virtual bool search_db_songs(bool);
    virtual bool search_add_tag_constraint(mpd_operator, mpd_tag_type, const char *);
    virtual std::vector<std::unique_ptr<song>> search_commit();

    connection(const connection &) = delete;
    connection &operator=(const connection &) = delete;
    connection(connection &&);
    connection &operator=(connection &&);
    virtual ~connection();

private:
    mpd_connection *m_connection;
};

} // namespace mpd

#endif