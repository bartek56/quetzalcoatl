#ifndef song_h
#define song_h

#include <mpd/client.h>

namespace mpd {

class song
{
public:
    virtual const char *get_tag(mpd_tag_type, unsigned);

    virtual explicit operator bool() const;
    song(mpd_song * = nullptr);
    song(song &);
    song(song &&);
    song &operator=(const song &);
    song &operator=(song &&);
    virtual ~song();

private:
    mpd_song *m_song;
};

} // namespace mpd

#endif