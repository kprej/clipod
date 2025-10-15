#pragma once

#include "albumMenu.h"
#include "artistMenu.h"
#include "mainMenu.h"
#include "nowPlaying.h"
#include "playbackControls.h"
#include "queue.h"
#include "trackMenu.h"

#include <ftxui/component/component.hpp>

#include <memory>

class uiLoop_t
{
public:
    ~uiLoop_t ();
    uiLoop_t ();

    bool run ();

private:
    void artistSelected (muuid::uuid const &id_);

    std::shared_ptr<ui::mainMenu_t> m_mainMenu;
    std::shared_ptr<ui::artistMenu_t> m_artistMenu;
    std::shared_ptr<ui::albumMenu_t> m_albumMenu;
    std::shared_ptr<ui::trackMenu_t> m_trackMenu;
    std::shared_ptr<ui::playbackControls_t> m_playbackControls;
    std::shared_ptr<ui::nowPlaying_t> m_nowPlaying;
    std::shared_ptr<ui::queue_t> m_queue;

    ftxui::Component m_component;

    std::deque<ftxui::Component> m_previousPages;
    ftxui::Component m_currentPage;

    bool m_run;
};
