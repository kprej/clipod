#include "playbackControls.h"
using namespace ui;

#include "clipod/brain.h"
using namespace clipod;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

playbackControls_t::~playbackControls_t ()
{
}

playbackControls_t::playbackControls_t (std::shared_ptr<playback_t> playback_)
    : m_seekBackwardButton (std::make_shared<pressedButton_t> ("<-"))
    , m_seekForwardButton (std::make_shared<pressedButton_t> ("->"))
    , m_skipForwardButton (std::make_shared<pressedButton_t> ("-->"))
    , m_skipBackwardButton (std::make_shared<pressedButton_t> ("<--"))
    , m_playButton (std::make_shared<binaryButton_t> ("|>", "||"))
    , m_stopButton (std::make_shared<pressedButton_t> ("[]"))
    , m_controlsComp (ftxui::Container::Horizontal ({m_playButton,
                                                     m_skipBackwardButton,
                                                     m_seekBackwardButton,
                                                     m_seekForwardButton,
                                                     m_skipForwardButton,
                                                     m_stopButton}))
{
    m_playButton->stateZeroFire.connect (&clipod::playback_t::play, playback_);
    m_playButton->stateOneFire.connect (&clipod::playback_t::pause, playback_);
    m_stopButton->buttonUp.connect (&clipod::playback_t::stop, playback_);
    m_skipForwardButton->buttonUp.connect (&clipod::playback_t::skip, playback_);
    m_skipBackwardButton->buttonUp.connect (&clipod::playback_t::back, playback_);

    playback_->stateChanged.connect (&playbackControls_t::handlePlaybackStateChanged,
                                     this);
}

ftxui::Element playbackControls_t::OnRender ()
{
    return m_controlsComp->Render ();
}

bool playbackControls_t::OnEvent (ftxui::Event event_)
{
    return m_controlsComp->OnEvent (event_);
}

void playbackControls_t::handlePlaybackStateChanged (playback_t::state_t state_)
{
    if (state_ == playback_t::state_t::PAUSED)
    {
        m_playButton->setState (true);
    }
    else if (state_ == playback_t::state_t::PLAYING)
    {
        m_playButton->setState (false);
    }
    else if (state_ == playback_t::state_t::STOPPED)
    {
        m_playButton->setState (false);
    }
}
