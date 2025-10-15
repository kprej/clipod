#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

namespace ui
{
class artistMenu_t : public ftxui::ComponentBase
{
public:
    ~artistMenu_t ();
    artistMenu_t ();

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<muuid::uuid> artistSelected;

private:
    void onEnter ();

    std::vector<std::string> m_artistStr;
    std::vector<muuid::uuid> m_artistId;

    int m_selected;
    ftxui::Component m_menu;
};
} // namespace ui
