#ifndef STATUS_H
#define STATUS_H

#include <mpd/client.h>

namespace mpd {

class Status
{
public:
    Status(mpd_status * = nullptr);
    virtual explicit operator bool();

    virtual unsigned get_queue_version();
    virtual unsigned get_queue_length();

    Status(const Status &) = delete;
    Status &operator=(const Status &) = delete;
    Status(Status &&);
    Status &operator=(Status &&);
    virtual ~Status();

private:
    mpd_status *m_status;
};

} // namespace mpd

#endif