#include "trackMenu.h"
using namespace ui;

#include "clipod/brain.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

trackMenu_t::~trackMenu_t ()
{
}

trackMenu_t::trackMenu_t ()
    : m_trackStr ()
    , m_trackId ()
    , m_selected (0)
    , m_menu ()
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::trackMenu_t::onEnter, this);

    m_menu = ftxui::Menu (&m_trackStr, &m_selected, mo);
}

void trackMenu_t::selectAlbum (muuid::uuid const &id_)
{
    m_trackStr.clear ();
    m_trackId.clear ();
    m_selected = 0;

    auto ai = clipod::DB ()->albumTrackInfo (id_);

    m_trackStr.reserve (ai.size ());
    m_trackId.reserve (ai.size ());

    for (auto track : ai)
    {
        m_trackId.push_back (track.id);
        m_trackStr.push_back (track.title);
    }
}

ftxui::Element trackMenu_t::OnRender ()
{
    return vbox (m_menu->Render () | vscroll_indicator | frame |
                 ftxui::size (HEIGHT, LESS_THAN, 12));
}

bool trackMenu_t::OnEvent (ftxui::Event event_)
{
    return m_menu->OnEvent (event_);
}

void trackMenu_t::onEnter ()
{
    trackSelected (m_trackId, m_selected);
}
