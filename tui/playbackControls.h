#pragma once

#include "clipod/playback.h"

#include "binaryButton.h"
#include "pressedButton.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <sigslot/signal.hpp>

#include <memory>

namespace ui
{
class playbackControls_t : public ftxui::ComponentBase
{
public:
    ~playbackControls_t ();
    playbackControls_t (std::shared_ptr<clipod::playback_t> playback_);

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

private:
    void handlePlaybackStateChanged (clipod::playback_t::state_t state_);

    std::shared_ptr<pressedButton_t> m_seekForwardButton;
    std::shared_ptr<pressedButton_t> m_seekBackwardButton;
    std::shared_ptr<pressedButton_t> m_skipForwardButton;
    std::shared_ptr<pressedButton_t> m_skipBackwardButton;
    std::shared_ptr<binaryButton_t> m_playButton;
    std::shared_ptr<pressedButton_t> m_stopButton;

    ftxui::Component m_controlsComp;
};
} // namespace ui
