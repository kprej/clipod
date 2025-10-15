#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

namespace ui
{
class albumMenu_t : public ftxui::ComponentBase
{
public:
    ~albumMenu_t ();
    albumMenu_t ();

    void selectArtist (muuid::uuid const &id_);

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<muuid::uuid> albumSelected;

private:
    void onEnter ();

    std::vector<std::string> m_albumStr;
    std::vector<muuid::uuid> m_albumId;

    int m_selected;
    ftxui::Component m_menu;
};
} // namespace ui
