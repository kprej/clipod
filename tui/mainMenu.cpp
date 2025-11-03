#include "mainMenu.h"
using namespace ui;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

mainMenu_t::~mainMenu_t ()
{
}

mainMenu_t::mainMenu_t ()
    : m_items ({"Artists", "Albums", "Settings"})
    , m_selected (0)
    , m_menu ()
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::mainMenu_t::onEnter, this);
    m_menu = ftxui::Menu (&m_items, &m_selected, mo);
}

ftxui::Element mainMenu_t::OnRender ()
{
    return m_menu->Render ();
}

bool mainMenu_t::OnEvent (ftxui::Event event_)
{
    return m_menu->OnEvent (event_);
}

void mainMenu_t::onEnter ()
{
    if (m_selected == 0)
        gotoArtistPage ();
    if (m_selected == 1)
        gotoAlbumPage ();
    if (m_selected == 2)
        gotoSettingsPage ();
}
