#include "uiLoop.h"
#include "playbackControls.h"

using namespace ui;

#include "clipod/brain.h"

#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp> // for hflow, paragraph, separator, hbox, vbox, filler, operator|, border, Element
#include <ftxui/dom/node.hpp> // for Render
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/screen.hpp>
using namespace ftxui;

uiLoop_t::~uiLoop_t ()
{
}

uiLoop_t::uiLoop_t ()
    : m_mainMenu (std::make_shared<mainMenu_t> ())
    , m_artistMenu (std::make_shared<artistMenu_t> ())
    , m_albumMenu (std::make_shared<albumMenu_t> ())
    , m_trackMenu (std::make_shared<trackMenu_t> ())
    , m_playbackControls (std::make_shared<playbackControls_t> (clipod::PB ()))
    , m_nowPlaying (std::make_shared<nowPlaying_t> (clipod::PB ()))
    , m_queue (std::make_shared<queue_t> ())
    , m_currentPage (m_mainMenu)
    , m_component ()
    , m_run (true)
{

    m_mainMenu->gotoArtistPage.connect (
        [this] ()
        {
            m_previousPages.push_back (m_currentPage);
            m_currentPage = m_artistMenu;
        });
    m_mainMenu->gotoAlbumPage.connect (
        [this] ()
        {
            m_previousPages.push_back (m_currentPage);
            m_currentPage = m_queue;
        });
    m_artistMenu->artistSelected.connect (
        [this] (muuid::uuid const &id_)
        {
            m_previousPages.push_back (m_currentPage);
            m_albumMenu->selectArtist (id_);
            m_currentPage = m_albumMenu;
        });

    m_albumMenu->albumSelected.connect (
        [this] (muuid::uuid const &id_)
        {
            m_previousPages.push_back (m_currentPage);
            m_trackMenu->selectAlbum (id_);
            m_currentPage = m_trackMenu;
        });

    m_trackMenu->trackSelected.connect (
        [this] (std::span<muuid::uuid> album_, int startPos_)
        {
            m_previousPages.push_back (m_currentPage);
            clipod::QU ()->enqueue (album_, startPos_);
            clipod::PB ()->play ();
            m_currentPage = m_nowPlaying;
        });

    m_component = Renderer (
        [&]
        {
            return ftxui::vbox ({m_currentPage->Render () | border,
                                 m_playbackControls->Render () | border});
        });

    m_component |= ftxui::CatchEvent (
        [&] (ftxui::Event event_)
        {
            if (event_ == Event::Backspace)
            {
                if (!m_previousPages.empty ())
                {
                    m_currentPage = m_previousPages.back ();
                    m_previousPages.pop_back ();

                    return true;
                }
            }
            if (event_ == Event::Escape)
            {
                m_run = false;
                return true;
            }

            if (m_playbackControls->OnEvent (event_))
                return true;

            if (m_currentPage->OnEvent (event_))
                return true;

            return false;
        });
}

bool uiLoop_t::run ()
{
    auto screens = ScreenInteractive::Fullscreen ();
    Loop loop (&screens, m_component);
    while (m_run)
    {
        std::this_thread::sleep_for (std::chrono::milliseconds (1000 / 15));
        screens.RequestAnimationFrame ();
        loop.RunOnce ();
    }

    return false;
}
