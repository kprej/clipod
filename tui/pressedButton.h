#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <sigslot/signal.hpp>

namespace ui
{
class pressedButton_t : public ftxui::ComponentBase
{
public:
    ~pressedButton_t ();
    pressedButton_t ();

    pressedButton_t (std::string_view text_);

    void setText (std::string_view text_);

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<> buttonDown;
    sigslot::signal<> buttonUp;

private:
    ftxui::Box m_box;
    std::string m_text;
    bool m_down;
};
} // namespace ui
