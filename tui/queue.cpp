#include "queue.h"

#include "clipod/brain.h"
using namespace clipod;

#include <ftxui/component/component.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp> // for hflow, paragraph, separator, hbox, vbox, filler, operator|, border, Element
#include <ftxui/dom/node.hpp> // for Render
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/screen.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

#include <tpropertymap.h>

ui::queue_t::~queue_t ()
{
}

ui::queue_t::queue_t ()
    : m_selected (0)
    , m_queue ()
    , m_table ()
{
    QU ()->queueUpdated.connect (&ui::queue_t::handleQueueUpdated, this);
    PB ()->nowPlaying.connect (&ui::queue_t::handleNowPlaying, this);

    MenuOption mo;
    mo.on_enter = std::bind (&ui::queue_t::onChange, this);
    m_table = ftxui::Menu (&m_queue, &m_selected, mo);
}

ftxui::Element ui::queue_t::OnRender ()
{
    return vbox (m_table->Render () | vscroll_indicator | frame |
                 ftxui::size (HEIGHT, LESS_THAN, 12));
}

bool ui::queue_t::OnEvent (ftxui::Event event_)
{
    return m_table->OnEvent (event_);
}

void ui::queue_t::handleQueueUpdated (std::span<muuid::uuid> tracks_)
{
    m_trackInfo.clear ();
    auto db = brain_t::bb ()->db;

    m_queue.clear ();
    for (auto const &id : tracks_)
    {
        auto track = db->trackInfo (id);
        m_trackInfo.push_back (track);
        m_queue.push_back (track.title);
    }
}

void ui::queue_t::handleNowPlaying (trackInfo_t const &trackInfo_)
{
    m_selected = trackInfo_.queuePos;
}

void ui::queue_t::onChange ()
{
    PB ()->skipTo (m_selected);
}
