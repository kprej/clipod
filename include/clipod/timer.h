#pragma once

#include <chrono>

namespace clipod
{
class timer_t
{
public:
    enum state_t
    {
        STOPPED,
        RUNNING,
        PAUSED,
        TIMEDOUT,
    };

    ~timer_t ();
    timer_t ();

    void setTime (std::chrono::seconds const &time_);

    bool timedout ();

    void start ();
    void pause ();
    void stop ();

private:
    void updateClockTime ();

    state_t m_state;
    std::chrono::milliseconds m_time;
    std::chrono::milliseconds m_clockTime;
    std::chrono::steady_clock::time_point m_lastPoint;
    std::optional<std::chrono::steady_clock> m_clock;
};
} // namespace clipod
