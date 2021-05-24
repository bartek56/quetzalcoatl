#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <mpd/client.h>

namespace mpdclient {

class directory
{
public:
    virtual const char *get_path() noexcept;

    virtual explicit operator bool() const;
    directory(mpd_directory * = nullptr);
    directory(directory &);
    directory(directory &&);
    directory &operator=(const directory &);
    directory &operator=(directory &&);
    virtual ~directory();

private:
    mpd_directory *m_directory{};
};

} // namespace mpdclient

#endif