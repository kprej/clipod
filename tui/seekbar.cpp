#include "seekbar.h"
using namespace ui;

#include "clipod/brain.h"
using namespace clipod;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

seekbar_t::~seekbar_t ()
{
}

seekbar_t::seekbar_t (std::shared_ptr<playback_t> playback_)
    : m_box ()
    , m_state (playback_t::state_t::STOPPED)
    , m_timeStamp (std::format ("{:%M:%S}", std::chrono::seconds (0)))
    , m_totalTime (std::format ("{:%M:%S}", std::chrono::seconds (0)))
    , m_length (std::chrono::seconds (0))
    , m_timePoint (std::chrono::milliseconds (0))
    , m_pos (100)
    , m_clock (std::nullopt)
{
    playback_->nowPlaying.connect (&ui::seekbar_t::handleNowPlaying, this);
    playback_->stateChanged.connect (&ui::seekbar_t::handlePlaybackStateChanged, this);
}

ftxui::Element seekbar_t::OnRender ()
{
    switch (m_state)
    {
    case playback_t::state_t::PAUSED:
        break;
    case playback_t::state_t::PLAYING:
    {
        auto now = m_clock->now ();

        m_timePoint +=
            std::chrono::duration_cast<std::chrono::milliseconds> (now - m_prevTime);
        m_timeStamp = std::format (
            "{:%M:%S}", std::chrono::duration_cast<std::chrono::seconds> (m_timePoint));

        m_pos = m_timePoint / m_length;

        m_prevTime = now;

        break;
    }

    case playback_t::state_t::STOPPED:
        return hbox ({paragraph (m_timeStamp),
                      separator (),
                      gauge (m_pos),
                      separator (),
                      paragraph (m_totalTime)}) |
               border;
    };

    return hbox ({paragraph (m_timeStamp),
                  separator (),
                  gauge (m_pos),
                  separator (),
                  paragraph (m_totalTime)}) |
           border;
}

bool seekbar_t::OnEvent (ftxui::Event event_)
{
    return false;
}

void seekbar_t::handleNowPlaying (clipod::trackInfo_t const &track_)
{
    if (track_.title.empty ())
    {
        m_totalTime = std::format ("{:%M:%S}", std::chrono::seconds (0));
        m_length = std::chrono::seconds (0);
        return;
    }

    m_length = track_.length;
    m_totalTime = std::format ("{:%M:%S}", m_length);

    m_clock = std::chrono::steady_clock ();
    m_prevTime = m_clock->now ();

    if (track_.timePoint)
        m_timePoint = track_.timePoint.value ();
}

void seekbar_t::handlePlaybackStateChanged (playback_t::state_t state_)
{
    m_state = state_;

    switch (m_state)
    {
    case playback_t::state_t::PAUSED:
        m_clock = std::nullopt;
        break;
    case playback_t::state_t::PLAYING:
    {
        m_clock = std::chrono::steady_clock ();
        m_prevTime = m_clock->now ();
        break;
    }
    case playback_t::state_t::STOPPED:
        m_clock = std::nullopt;
        m_timePoint = std::chrono::milliseconds (0);
        break;
    };
}
