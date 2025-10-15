#pragma once

namespace clipod
{
class playbackControls_t
{
public:
    ~playbackControls_t ();
    playbackControls_t ();

    void play ();
    void pause ();

    void prevTrack ();
    void nextTrack ();

    void stop ();

    void skip ();

    void startSeek (bool forward_);
    void stopSeek ();
};
} // namespace clipod
