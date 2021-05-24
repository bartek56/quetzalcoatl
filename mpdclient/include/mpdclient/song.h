#ifndef song_h
#define song_h

#include <mpd/client.h>

namespace mpdclient {

class song
{
public:
    virtual const char *get_tag(mpd_tag_type, unsigned) noexcept;

    virtual explicit operator bool() const;
    song(mpd_song * = nullptr);
    song(song &);
    song(song &&);
    song &operator=(const song &);
    song &operator=(song &&);
    virtual ~song();

private:
    mpd_song *m_song{};
};

} // namespace mpdclient

#endif