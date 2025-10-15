#pragma once

#include "clipod/playback.h"
#include "clipod/track.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <sigslot/signal.hpp>

#include <chrono>
#include <optional>

namespace ui
{
class seekbar_t : public ftxui::ComponentBase
{
public:
    ~seekbar_t ();
    seekbar_t (std::shared_ptr<clipod::playback_t> playback_);

    ftxui::Element OnRender () override;
    bool OnEvent (ftxui::Event event_) override;

private:
    void handleNowPlaying (clipod::trackInfo_t const &track_);
    void handlePlaybackStateChanged (clipod::playback_t::state_t state_);

    ftxui::Box m_box;

    clipod::playback_t::state_t m_state;
    std::string m_timeStamp;
    std::string m_totalTime;
    std::chrono::seconds m_length;
    std::chrono::duration<float> m_timePoint;
    std::chrono::steady_clock::time_point m_prevTime;
    std::optional<std::chrono::steady_clock> m_clock;

    float m_pos;
};
} // namespace ui
