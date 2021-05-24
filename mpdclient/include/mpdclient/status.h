#ifndef STATUS_H
#define STATUS_H

#include <mpd/client.h>

namespace mpdclient {

class status
{
public:
    status(mpd_status * = nullptr);
    virtual explicit operator bool() const;

    virtual unsigned get_queue_version() noexcept;
    virtual unsigned get_queue_length() noexcept;
    virtual unsigned get_total_time() noexcept;
    virtual unsigned get_elapsed_ms() noexcept;
    virtual bool get_random() noexcept;
    virtual bool get_repeat() noexcept;
    virtual int get_volume() noexcept;
    virtual unsigned get_crossfade() noexcept;
    virtual mpd_state get_state() noexcept;
    virtual int get_song_id() noexcept;

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