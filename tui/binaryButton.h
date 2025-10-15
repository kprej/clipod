#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>

#include <sigslot/signal.hpp>

namespace ui
{
class binaryButton_t : public ftxui::ComponentBase
{
public:
    ~binaryButton_t ();
    binaryButton_t ();
    binaryButton_t (std::string_view stateZero_, std::string_view stateOne_);

    void setStateZeroText (std::string_view text_);
    void setStateOneText (std::string_view text_);

    void setState (bool on_);

    ftxui::Element OnRender () override;

    bool OnEvent (ftxui::Event event_) override;

    sigslot::signal<> stateZeroFire;
    sigslot::signal<> stateOneFire;

private:
    ftxui::Box m_box;
    std::string m_stateZeroText;
    std::string m_stateOneText;

    bool m_state;
};
} // namespace ui
