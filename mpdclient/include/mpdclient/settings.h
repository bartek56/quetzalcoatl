#ifndef SETTINGS_H
#define SETTINGS_H

#include <mpd/client.h>

namespace mpdclient {

class settings
{
public:
    settings(mpd_settings * = nullptr);
    virtual explicit operator bool() const;

    virtual const char *get_host() noexcept;
    virtual unsigned get_port() noexcept;
    virtual unsigned get_timeout_ms() noexcept;
    virtual const char *get_password() noexcept;

    settings(const settings &) = delete;
    settings &operator=(const settings &) = delete;
    settings(settings &&);
    settings &operator=(settings &&);
    virtual ~settings();

private:
    mpd_settings *m_settings{};
};

}; // namespace mpdclient
#endif