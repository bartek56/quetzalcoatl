#ifndef STATUS_H
#define STATUS_H

#include <mpd/client.h>

namespace mpdclient {

class status
{
public:
    status(mpd_status * = nullptr);
    virtual explicit operator bool() const;

    virtual unsigned get_queue_version();
    virtual unsigned get_queue_length();
    virtual unsigned get_total_time();
    virtual unsigned get_elapsed_ms();
    virtual bool get_random();
    virtual bool get_repeat();
    virtual int get_volume();
    virtual unsigned get_crossfade();
    virtual mpd_state get_state();
    virtual int get_song_id();

    status(const status &) = delete;
    status &operator=(const status &) = delete;
    status(status &&);
    status &operator=(status &&);
    virtual ~status();

private:
    mpd_status *m_status{};
};

} // namespace mpdclient

#endif