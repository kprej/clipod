#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <sigslot/signal.hpp>

namespace ui
{
class mainMenu_t : public ftxui::ComponentBase
{
public:
    ~mainMenu_t ();
    mainMenu_t ();

    ftxui::Element OnRender () override;
    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<> gotoArtistPage;
    sigslot::signal<> gotoAlbumPage;
    sigslot::signal<> gotoSettingsPage;

private:
    void onEnter ();

    std::vector<std::string> m_items;
    int m_selected;
    ftxui::Component m_menu;
};
} // namespace ui
