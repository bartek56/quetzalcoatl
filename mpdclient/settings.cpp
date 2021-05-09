#include "mpdclient/settings.h"

mpd::settings::settings(mpd_settings *settings_ptr)
    : m_settings(settings_ptr)
{}

mpd::settings::operator bool() const
{
    return m_settings;
}

const char *mpd::settings::get_host()
{
    return mpd_settings_get_host(m_settings);
}

unsigned mpd::settings::get_port()
{
    return mpd_settings_get_port(m_settings);
}

unsigned mpd::settings::get_timeout_ms()
{
    return mpd_settings_get_timeout_ms(m_settings);
}

const char *mpd::settings::get_password()
{
    return mpd_settings_get_password(m_settings);
}

mpd::settings::settings(mpd::settings &&other)
    : m_settings(other.m_settings)
{
    other.m_settings = nullptr;
}

mpd::settings &mpd::settings::operator=(mpd::settings &&other)
{
    if (this != &other) {
        if (m_settings) {
            mpd_settings_free(m_settings);
        }

        m_settings = other.m_settings;
        other.m_settings = nullptr;
    }
    return *this;
}

mpd::settings::~settings()
{
    if (m_settings) {
        mpd_settings_free(m_settings);
    }
}