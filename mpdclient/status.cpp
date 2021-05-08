#include "mpdclient/status.h"

mpd::Status::Status(mpd_status *status)
    : m_status{status}
{}

mpd::Status::operator bool()
{
    return bool(m_status);
}

unsigned mpd::Status::get_queue_version()
{
    return mpd_status_get_queue_version(m_status);
}

unsigned mpd::Status::get_queue_length()
{
    return mpd_status_get_queue_length(m_status);
}

mpd::Status::Status(mpd::Status &&other)
    : m_status(other.m_status)
{
    other.m_status = nullptr;
}

mpd::Status &mpd::Status::operator=(mpd::Status &&other)
{
    if (this != &other) {
        if (m_status) {
            mpd_status_free(m_status);
        }

        m_status = other.m_status;
        other.m_status = nullptr;
    }
    return *this;
}

mpd::Status::~Status()
{
    if (m_status) {
        mpd_status_free(m_status);
    }
}
