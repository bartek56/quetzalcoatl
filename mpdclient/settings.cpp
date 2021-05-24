#include "mpdclient/settings.h"

mpdclient::settings::settings(mpd_settings *settings_ptr)
    : m_settings(settings_ptr)
{}

mpdclient::settings::operator bool() const
{
    return m_settings;
}

const char *mpdclient::settings::get_host() noexcept
{
    return mpd_settings_get_host(m_settings);
}

unsigned mpdclient::settings::get_port() noexcept
{
    return mpd_settings_get_port(m_settings);
}

unsigned mpdclient::settings::get_timeout_ms() noexcept
{
    return mpd_settings_get_timeout_ms(m_settings);
}

const char *mpdclient::settings::get_password() noexcept
{
    return mpd_settings_get_password(m_settings);
}

mpdclient::settings::settings(mpdclient::settings &&other)
    : m_settings(other.m_settings)
{
    other.m_settings = nullptr;
}

mpdclient::settings &mpdclient::settings::operator=(mpdclient::settings &&other)
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

mpdclient::settings::~settings()
{
    if (m_settings) {
        mpd_settings_free(m_settings);
    }
}