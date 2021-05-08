#ifndef song_h
#define song_h

#include <mpd/client.h>

namespace mpd {

class Song
{
public:
    virtual const char *get_tag(mpd_tag_type, unsigned);

    operator bool();
    Song(mpd_song * = nullptr);
    Song(Song &);
    Song(Song &&);
    Song &operator=(const Song &);
    Song &operator=(Song &&);
    virtual ~Song();

private:
    mpd_song *m_song;
};

} // namespace mpd

#endif