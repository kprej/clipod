#include "pressedButton.h"
using namespace ui;

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

pressedButton_t::~pressedButton_t ()
{
}

pressedButton_t::pressedButton_t ()
    : m_box ()
    , m_text ("")
    , m_down (false)
{
}

pressedButton_t::pressedButton_t (std::string_view text_)
    : m_box ()
    , m_text (text_)
    , m_down (false)
{
}

void pressedButton_t::setText (std::string_view text_)
{
    m_text = text_;
}

ftxui::Element pressedButton_t::OnRender ()
{
    return ftxui::border (ftxui::text (m_text)) | ftxui::reflect (m_box);
}

bool pressedButton_t::OnEvent (ftxui::Event event_)
{
    if (event_.is_mouse ())
    {
        if (!CaptureMouse (event_))
            return false;

        if (m_box.Contain (event_.mouse ().x, event_.mouse ().y))
            if (event_.mouse ().button == ftxui::Mouse::Left &&
                event_.mouse ().motion == ftxui::Mouse::Pressed)
            {
                if (!m_down)
                    buttonDown ();

                m_down = true;
                return true;
            }

        if (event_.mouse ().button == ftxui::Mouse::Left &&
            event_.mouse ().motion == ftxui::Mouse::Released && m_down)
        {
            buttonUp ();
            m_down = false;
            return true;
        }

        return false;
    }

    return false;
}
