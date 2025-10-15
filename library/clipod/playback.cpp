#include "clipod/playback.h"
#include "clipod/brain.h"
#include "clipod/soundBuffer.h"
#include "libavutil/log.h"
#include "modern-uuid/uuid.h"

using namespace clipod;

#include <spdlog/spdlog.h>
// OpenAL Below here
#include "AL/al.h"
#include "AL/alc.h"

playback_t::~playback_t ()
{
}

playback_t::playback_t ()
    : currentState (STOPPED)
{
    m_device = alcOpenDevice (nullptr);
    m_context = alcCreateContext (m_device, nullptr);
    alcMakeContextCurrent (m_context);
    assert (m_context);

    if (alGetError () != AL_NO_ERROR)
        throw std::runtime_error {"alGenBuffers failed"};

    av_log_set_level (0);
}

void playback_t::unload ()
{
    if (m_nowPlaying)
        m_nowPlaying->unload ();

    alcDestroyContext (m_context);
    alcCloseDevice (m_device);
}

void playback_t::play ()
{
    if (currentState == PLAYING)
        return;

    if (!m_nowPlaying)
    {
        auto qu = brain_t::bb ()->qu;

        if (auto id = qu->currentTrack ())
            m_nowPlaying = loadBuffer (id.value ());
        else
            return;
    }

    m_nowPlaying->play ();

    updateNowPlayingInfo ();

    currentState = PLAYING;
    stateChanged (currentState);
}

void playback_t::pause ()
{
    if (currentState == PAUSED)
        return;

    if (!m_nowPlaying)
        return;

    m_nowPlaying->pause ();

    currentState = PAUSED;

    updateNowPlayingInfo ();
    stateChanged (currentState);
}

void playback_t::seek (int seek_)
{
    if (!m_nowPlaying)
        return;

    m_nowPlaying->seek (seek_);
}

void playback_t::back ()
{
    if (!playPrev ())
        return;

    updateNowPlayingInfo ();

    m_nowPlaying->play ();
}

void playback_t::skip ()
{
    if (!playNext ())
        return;

    updateNowPlayingInfo ();

    m_nowPlaying->play ();
}

void playback_t::skipTo (uint16_t pos_)
{
    if (m_nowPlaying)
    {
        m_nowPlaying->pause ();
    }

    auto skipped = QU ()->skipTo (pos_);

    if (!skipped)
        return;

    auto next = loadBuffer (skipped.value ());

    m_nowPlaying = next;

    updateNowPlayingInfo ();

    m_nowPlaying->play ();
}

void playback_t::stop ()
{
    if (!m_nowPlaying)
        return;

    m_nowPlaying->pause ();

    m_nowPlaying->stop ();
    m_nowPlaying = nullptr;

    currentState = STOPPED;

    updateNowPlayingInfo ();

    stateChanged (currentState);
}

void playback_t::updateNowPlayingInfo ()
{
    if (!m_nowPlaying)
    {
        nowPlaying (trackInfo_t {});
        return;
    }

    if (m_nowPlaying->id () != m_trackInfo.id)
        m_trackInfo = DB ()->trackInfo (m_nowPlaying->id ());

    m_trackInfo.timePoint = m_nowPlaying->timePoint ();
    m_trackInfo.queuePos = QU ()->pos ();

    nowPlaying (m_trackInfo);
}

bool playback_t::playNext ()
{
    if (m_nowPlaying)
    {
        m_nowPlaying->pause ();
    }

    if (QU ()->empty ())
        return false;

    auto id = QU ()->moveForward ();

    if (!id)
        return false;

    auto next = loadBuffer (id.value ());

    m_nowPlaying = next;

    return true;
}

bool playback_t::playPrev ()
{
    if (m_nowPlaying)
    {
        m_nowPlaying->pause ();
    }

    if (QU ()->empty ())
        return false;

    auto id = QU ()->moveBack ();
    if (!id)
        return false;

    auto next = loadBuffer (id.value ());

    m_nowPlaying = next;

    return true;
}

void playback_t::startSeek (bool forward_)
{
    m_nowPlaying->pause ();
}

void playback_t::stopSeek ()
{
    m_nowPlaying->play ();
}

std::shared_ptr<soundBuffer_t> playback_t::loadBuffer (muuid::uuid const &id_)
{
    auto returnValue = std::make_shared<soundBuffer_t> (id_);

    returnValue->load ();

    return returnValue;
}
