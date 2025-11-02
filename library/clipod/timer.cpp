#include "clipod/timer.h"

clipod::timer_t::~timer_t ()
{
}

clipod::timer_t::timer_t ()
    : m_state (STOPPED)
    , m_time ()
    , m_clockTime (0)
    , m_lastPoint ()
    , m_clock (std::nullopt)
{
}

void clipod::timer_t::setTime (std::chrono::seconds const &time_)
{
    m_time = time_;
}

bool clipod::timer_t::timedout ()
{
    updateClockTime ();

    return m_state == TIMEDOUT;
}

void clipod::timer_t::start ()
{
    m_clock = std::chrono::steady_clock ();
    m_lastPoint = m_clock->now ();
    m_state = RUNNING;
}

void clipod::timer_t::pause ()
{
    m_clock = std::nullopt;
    m_state = PAUSED;
}

void clipod::timer_t::stop ()
{
    m_clock = std::nullopt;
    m_clockTime = std::chrono::milliseconds (0);
    m_state = STOPPED;
}

void clipod::timer_t::updateClockTime ()
{
    if (m_state != RUNNING)
        return;

    auto now = m_clock->now ();

    m_clockTime +=
        std::chrono::duration_cast<std::chrono::milliseconds> (now - m_lastPoint);

    m_lastPoint = now;

    if (m_time <= m_clockTime)
    {
        m_clock = std::nullopt;
        m_state = TIMEDOUT;
    }
}
