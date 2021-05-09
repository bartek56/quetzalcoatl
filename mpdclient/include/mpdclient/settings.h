#ifndef SETTINGS_H
#define SETTINGS_H

#include <mpd/client.h>

namespace mpd {

class settings
{
public:
    settings(mpd_settings * = nullptr);
    virtual explicit operator bool() const;
    virtual const char *get_host();
    virtual unsigned get_port();
    virtual unsigned get_timeout_ms();
    virtual const char *get_password();

    settings(const settings &) = delete;
    settings &operator=(const settings &) = delete;
    settings(settings &&);
    settings &operator=(settings &&);
    virtual ~settings();

private:
    mpd_settings *m_settings{};
};

}; // namespace mpd
#endif