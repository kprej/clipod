#pragma once

#include "clipod/track.h"

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <fileref.h>
#include <tag.h>

namespace ui
{

class queue_t : public ftxui::ComponentBase
{
public:
    ~queue_t ();
    queue_t ();

    ftxui::Element OnRender () override;
    bool OnEvent (ftxui::Event event_) override;

private:
    void handleQueueUpdated (std::span<muuid::uuid> tracks_);
    void handleNowPlaying (clipod::trackInfo_t const &trackInfo_);

    void onChange ();

    int m_selected;
    std::vector<std::string> m_queue;
    std::vector<clipod::trackInfo_t> m_trackInfo;
    ftxui::Component m_table;
};
} // namespace ui
