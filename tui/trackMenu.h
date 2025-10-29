#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <modern-uuid/uuid.h>

#include <sigslot/signal.hpp>

namespace ui
{
class trackMenu_t : public ftxui::ComponentBase
{
public:
    ~trackMenu_t ();
    trackMenu_t ();

    void selectAlbum (muuid::uuid const &id_);

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<std::span<muuid::uuid>, int> trackSelected;

private:
    void onEnter ();

    std::vector<std::string> m_trackStr;
    std::vector<muuid::uuid> m_trackId;

    int m_selected;
    ftxui::Component m_menu;
};
} // namespace ui
