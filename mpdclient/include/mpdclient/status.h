#ifndef STATUS_H
#define STATUS_H

#include <mpd/client.h>

namespace mpd {

class status
{
public:
    status(mpd_status * = nullptr);
    virtual explicit operator bool();

    virtual unsigned get_queue_version();
    virtual unsigned get_queue_length();

    status(const status &) = delete;
    status &operator=(const status &) = delete;
    status(status &&);
    status &operator=(status &&);
    virtual ~status();

private:
    mpd_status *m_status;
};

} // namespace mpd

#endif