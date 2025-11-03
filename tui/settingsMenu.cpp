#include "settingsMenu.h"
#include "clipod/brain.h"
#include "fileBrowser.h"
#include <memory>
using namespace ui;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

settingsMenu_t::~settingsMenu_t ()
{
}

settingsMenu_t::settingsMenu_t ()
    : m_settingsStr ({"Set Music Directory", "Scan Music"})
    , m_artistId ()
    , m_selected (0)
    , m_menu ()
    , m_state (SELECTING)
    , m_fileBrowser (nullptr)
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::settingsMenu_t::onEnter, this);

    m_menu = ftxui::Menu (&m_settingsStr, &m_selected, mo);
}

ftxui::Element settingsMenu_t::OnRender ()
{
    switch (m_state)
    {
    case SELECTING:
        return vbox (m_menu->Render () | vscroll_indicator | frame |
                     ftxui::size (HEIGHT, LESS_THAN, 12));
    case MUSIC_DIR:
        return m_fileBrowser->Render ();
    case SCAN:
    }

    return {};
}

bool settingsMenu_t::OnEvent (ftxui::Event event_)
{
    switch (m_state)
    {
    case SELECTING:
        return m_menu->OnEvent (event_);
    case MUSIC_DIR:
        return m_fileBrowser->OnEvent (event_);
    case SCAN:
    }

    return false;
}

void settingsMenu_t::onEnter ()
{
    if (m_selected == 0)
    {
        m_state = MUSIC_DIR;

        m_fileBrowser = std::make_shared<ui::fileBrowser_t> ();

        m_fileBrowser->directorySelected.connect (
            [this] (std::filesystem::path const &path_)
            {
                spdlog::debug ("Music dir: {}", path_.string ());
                m_state = SELECTING;
                clipod::SETTINGS ()->setMusicDir (path_);
            });
    }

    if (m_selected == 1)
    {
        clipod::DB ()->scanDirectory (clipod::SETTINGS ()->musicDir (), true);
    }
}
