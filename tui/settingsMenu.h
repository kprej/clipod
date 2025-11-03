#pragma once

#include "fileBrowser.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

namespace ui
{
class settingsMenu_t : public ftxui::ComponentBase
{
public:
    ~settingsMenu_t ();
    settingsMenu_t ();

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

private:
    enum state_t
    {
        SELECTING,
        MUSIC_DIR,
        SCAN,
    };

    void onEnter ();

    std::vector<std::string> m_settingsStr;
    std::vector<muuid::uuid> m_artistId;

    int m_selected;
    ftxui::Component m_menu;

    state_t m_state;

    std::shared_ptr<ui::fileBrowser_t> m_fileBrowser;
};
} // namespace ui
