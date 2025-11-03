#include "fileBrowser.h"
#include <filesystem>
using namespace ui;

#include "clipod/brain.h"

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
using namespace ftxui;

#include <spdlog/spdlog.h>

fileBrowser_t::~fileBrowser_t ()
{
}

fileBrowser_t::fileBrowser_t ()
    : m_directoryStr ()
    , m_selected (0)
    , m_menu ()
{
    MenuOption mo;
    mo.on_enter = std::bind (&ui::fileBrowser_t::onEnter, this);

    m_menu = ftxui::Menu (&m_directoryStr, &m_selected, mo);

    setup (std::filesystem::current_path ());
}

ftxui::Element fileBrowser_t::OnRender ()
{
    return vbox (m_menu->Render () | vscroll_indicator | frame |
                 ftxui::size (HEIGHT, LESS_THAN, 12));
}

bool fileBrowser_t::OnEvent (ftxui::Event event_)
{
    return m_menu->OnEvent (event_);
}

void fileBrowser_t::setup (std::filesystem::path const &path_)
{
    m_currentPath = path_;

    m_directoryStr.clear ();

    m_directoryStr.push_back (".");

    m_directoryPaths.push_back (path_);
    m_directoryPaths.push_back (path_.parent_path ());

    m_directoryStr.push_back ("..");

    for (auto const &entry : std::filesystem::directory_iterator {path_})
    {
        if (!entry.is_directory ())
            continue;

        m_directoryStr.push_back (entry.path ().filename ().string ());
        m_directoryPaths.push_back (entry.path ());
    }
}

void fileBrowser_t::onEnter ()
{
    if (m_selected == 0)
    {
        directorySelected (m_currentPath);
        return;
    }

    if (m_selected == 1)
    {
        setup (m_currentPath.parent_path ());
        return;
    }

    setup (m_directoryPaths.at (m_selected - 2));
}
