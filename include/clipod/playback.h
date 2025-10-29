#pragma once

#include "clipod/soundBuffer.h"
#include "clipod/track.h"

#include <alc.h>

#include <chrono>
#include <modern-uuid/uuid.h>

#include <fileref.h>

#include <sigslot/signal.hpp>

#include <memory>

namespace clipod
{
struct playback_t
{
public:
    enum state_t
    {
        PLAYING,
        PAUSED,
        STOPPED,
    };

    ~playback_t ();
    playback_t ();

    void unload ();

    state_t currentState;

    sigslot::signal<trackInfo_t const &> nowPlaying;
    sigslot::signal<trackInfo_t const &> queuedUp;
    sigslot::signal<state_t> stateChanged;

    void play ();
    void pause ();
    void seek (int seek_);
    void back ();
    void skip ();
    void skipTo (uint16_t pos_);
    void stop ();

    void startSeek (bool forward_);
    void stopSeek ();

private:
    void updateNowPlayingInfo ();

    void readyUpNextTrack ();

    bool playNext ();
    bool playPrev ();

    std::shared_ptr<soundBuffer_t> loadBuffer (muuid::uuid const &id_);

    ALCdevice *m_device;
    ALCcontext *m_context;

    trackInfo_t m_trackInfo;
    std::shared_ptr<soundBuffer_t> m_nowPlaying;
    std::shared_ptr<soundBuffer_t> m_nextTrack;

    timer_t m_nextUpTimer;
};
} // namespace clipod
