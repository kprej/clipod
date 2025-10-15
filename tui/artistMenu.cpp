#include "artistMenu.h"
using namespace ui;

#include "clipod/brain.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

artistMenu_t::~artistMenu_t ()
{
}

artistMenu_t::artistMenu_t ()
    : m_artistStr ()
    , m_artistId ()
    , m_selected (0)
    , m_menu ()
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::artistMenu_t::onEnter, this);

    auto ai = clipod::DB ()->allArtistInfo ();

    m_artistStr.reserve (ai.size ());
    m_artistId.reserve (ai.size ());

    for (auto artist : ai)
    {
        m_artistId.push_back (artist.id);
        m_artistStr.push_back (artist.name);
    }

    m_menu = ftxui::Menu (&m_artistStr, &m_selected, mo);
}

ftxui::Element artistMenu_t::OnRender ()
{
    return vbox (m_menu->Render () | vscroll_indicator | frame |
                 ftxui::size (HEIGHT, LESS_THAN, 12));
}

bool artistMenu_t::OnEvent (ftxui::Event event_)
{
    return m_menu->OnEvent (event_);
}

void artistMenu_t::onEnter ()
{
    artistSelected (m_artistId.at (m_selected));
}
