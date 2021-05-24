#include "mpdclient/directory.h"

const char *mpdclient::directory::get_path() noexcept
{
    return mpd_directory_get_path(m_directory);
}

mpdclient::directory::operator bool() const
{
    return m_directory;
}

mpdclient::directory::directory(mpd_directory *directory)
    : m_directory(directory)
{}

mpdclient::directory::directory(mpdclient::directory &other)
{
    m_directory = other.m_directory ? mpd_directory_dup(other.m_directory) : other.m_directory;
}

mpdclient::directory::directory(mpdclient::directory &&other)
{
    m_directory = other.m_directory;
    other.m_directory = nullptr;
}

mpdclient::directory &mpdclient::directory::operator=(const mpdclient::directory &other)
{
    if (this != &other) {
        m_directory = other.m_directory ? mpd_directory_dup(other.m_directory) : other.m_directory;
    }
    return *this;
}

mpdclient::directory &mpdclient::directory::operator=(mpdclient::directory &&other)
{
    m_directory = other.m_directory;
    other.m_directory = nullptr;
    return *this;
}

mpdclient::directory::~directory()
{
    if (m_directory) {
        mpd_directory_free(m_directory);
    }
}