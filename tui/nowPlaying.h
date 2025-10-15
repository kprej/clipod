#pragma once

#include "seekbar.h"

#include "clipod/track.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <fileref.h>
#include <tag.h>

namespace ui
{

class nowPlaying_t : public ftxui::ComponentBase
{
public:
    ~nowPlaying_t ();
    nowPlaying_t (std::shared_ptr<clipod::playback_t> playback_);

    ftxui::Element OnRender () override;
    bool OnEvent (ftxui::Event event_) override;

private:
    ftxui::Element titleBar ();

    void handleNowPlaying (clipod::trackInfo_t const &track_);
    void handleQueueUpdated (std::span<muuid::uuid> tracks_);

    std::shared_ptr<seekbar_t> m_seekbar;
    clipod::trackInfo_t m_trackInfo;
    size_t m_queueSize;
};
} // namespace ui
