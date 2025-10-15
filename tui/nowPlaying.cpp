#include "nowPlaying.h"

#include "clipod/brain.h"
using namespace clipod;

#include <ctrl-c.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp> // for hflow, paragraph, separator, hbox, vbox, filler, operator|, border, Element
#include <ftxui/dom/node.hpp> // for Render
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/screen.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

#include <memory>
#include <tpropertymap.h>

ui::nowPlaying_t::~nowPlaying_t ()
{
}

ui::nowPlaying_t::nowPlaying_t (std::shared_ptr<clipod::playback_t> playback_)
    : m_seekbar (std::make_shared<seekbar_t> (playback_))
    , m_trackInfo ()
{
    playback_->nowPlaying.connect (&ui::nowPlaying_t::handleNowPlaying, this);
    QU ()->queueUpdated.connect (&ui::nowPlaying_t::handleQueueUpdated, this);
}

ftxui::Element ui::nowPlaying_t::OnRender ()
{
    return vbox ({
        titleBar (),
        m_seekbar->Render (),
    });
}

bool ui::nowPlaying_t::OnEvent (ftxui::Event event_)
{
    m_seekbar->OnEvent (event_);
    return false;
}

Element ui::nowPlaying_t::titleBar ()
{
    if (m_trackInfo.title.empty ())
    {
        return vbox ({paragraph (" "),
                      separator (),
                      paragraph (" "),
                      separator (),
                      paragraph (" "),
                      separator (),
                      paragraph (" ")}) |
               border;
    }

    std::string pos =
        std::to_string (m_trackInfo.queuePos + 1) + " | " + std::to_string (m_queueSize);

    return vbox ({paragraph (m_trackInfo.title),
                  separator (),
                  paragraph (m_trackInfo.artist),
                  separator (),
                  paragraph (m_trackInfo.album),
                  separator (),
                  paragraph (pos)}) |
           border;
}

void ui::nowPlaying_t::handleNowPlaying (trackInfo_t const &track_)
{
    m_trackInfo = track_;
}

void ui::nowPlaying_t::handleQueueUpdated (std::span<muuid::uuid> tracks_)
{
    m_queueSize = tracks_.size ();
}
