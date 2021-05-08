#include "mpdclient/status.h"

mpd::status::status(mpd_status *status)
    : m_status{status}
{}

mpd::status::operator bool()
{
    return bool(m_status);
}

unsigned mpd::status::get_queue_version()
{
    return mpd_status_get_queue_version(m_status);
}

unsigned mpd::status::get_queue_length()
{
    return mpd_status_get_queue_length(m_status);
}

mpd::status::status(mpd::status &&other)
    : m_status(other.m_status)
{
    other.m_status = nullptr;
}

mpd::status &mpd::status::operator=(mpd::status &&other)
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

mpd::status::~status()
{
    if (m_status) {
        mpd_status_free(m_status);
    }
}
