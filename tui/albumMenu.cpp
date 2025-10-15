#include "albumMenu.h"
using namespace ui;

#include "clipod/brain.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

albumMenu_t::~albumMenu_t ()
{
}

albumMenu_t::albumMenu_t ()
    : m_albumStr ()
    , m_albumId ()
    , m_selected (0)
    , m_menu ()
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::albumMenu_t::onEnter, this);

    m_menu = ftxui::Menu (&m_albumStr, &m_selected, mo);
}

void albumMenu_t::selectArtist (muuid::uuid const &id_)
{
    m_albumStr.clear ();
    m_albumId.clear ();

    auto ai = clipod::DB ()->artistAlbumsInfo (id_);

    m_albumStr.reserve (ai.size ());
    m_albumId.reserve (ai.size ());

    for (auto album : ai)
    {
        m_albumId.push_back (album.id);
        m_albumStr.push_back (album.name);
    }
}

ftxui::Element albumMenu_t::OnRender ()
{
    return vbox (m_menu->Render () | vscroll_indicator | frame |
                 ftxui::size (HEIGHT, LESS_THAN, 12));
}

bool albumMenu_t::OnEvent (ftxui::Event event_)
{
    return m_menu->OnEvent (event_);
}

void albumMenu_t::onEnter ()
{
    albumSelected (m_albumId.at (m_selected));
}
