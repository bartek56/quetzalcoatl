#include "mpdclient/status.h"

mpdclient::status::status(mpd_status *status)
    : m_status{status}
{}

mpdclient::status::operator bool() const
{
    return bool(m_status);
}

unsigned mpdclient::status::get_queue_version()
{
    return mpd_status_get_queue_version(m_status);
}

unsigned mpdclient::status::get_queue_length()
{
    return mpd_status_get_queue_length(m_status);
}

mpdclient::status::status(mpdclient::status &&other)
    : m_status(other.m_status)
{
    other.m_status = nullptr;
}

mpdclient::status &mpdclient::status::operator=(mpdclient::status &&other)
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

mpdclient::status::~status()
{
    if (m_status) {
        mpd_status_free(m_status);
    }
}
