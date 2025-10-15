#include "binaryButton.h"
using namespace ui;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

#include <spdlog/spdlog.h>

binaryButton_t::~binaryButton_t ()
{
}

binaryButton_t::binaryButton_t ()
    : m_box ()
    , m_stateZeroText ("")
    , m_stateOneText ("")
    , m_state (false)
{
}

binaryButton_t::binaryButton_t (std::string_view stateZero_, std::string_view stateOne_)
    : m_box ()
    , m_stateZeroText (stateZero_)
    , m_stateOneText (stateOne_)
    , m_state (false)
{
}

void binaryButton_t::setStateZeroText (std::string_view text_)
{
    m_stateZeroText = text_;
}

void binaryButton_t::setStateOneText (std::string_view text_)
{
    m_stateOneText = text_;
}

void binaryButton_t::setState (bool on_)
{
    m_state = on_;
}

ftxui::Element binaryButton_t::OnRender ()
{
    return ftxui::border (ftxui::text (m_state ? m_stateOneText : m_stateZeroText)) |
           ftxui::reflect (m_box);
}

bool binaryButton_t::OnEvent (ftxui::Event event_)
{
    if (event_.is_mouse ())
    {
        if (!CaptureMouse (event_))
            return false;

        if (m_box.Contain (event_.mouse ().x, event_.mouse ().y))
        {

            if (event_.mouse ().button == ftxui::Mouse::Left &&
                event_.mouse ().motion == ftxui::Mouse::Pressed)
            {
                m_state ? stateOneFire () : stateZeroFire ();

                m_state = !m_state;
                return true;
            }
        }
    }

    return false;
}
