#include "clipod/queue.h"
#include "clipod/playback.h"
#include <istream>
using namespace clipod;

queue_t::~queue_t ()
{
}

queue_t::queue_t ()
    : m_queue ()
    , m_currentPos (m_queue.end ())
{
}

void queue_t::enqueue (std::span<muuid::uuid> album_, int startPos_)
{
    m_queue.clear ();

    m_queue.insert (m_queue.end (), album_.cbegin (), album_.cend ());

    m_currentPos = m_queue.begin () + startPos_;

    queueUpdated (m_queue);
}

void queue_t::enqueue (muuid::uuid const &track_)
{
    auto dist = std::distance (m_queue.begin (), m_currentPos);

    m_queue.push_back (track_);

    m_currentPos = m_queue.begin () + dist;

    queueUpdated (m_queue);
}

void queue_t::enqueue (std::span<muuid::uuid> track_)
{
    auto dist = std::distance (m_queue.begin (), m_currentPos);

    m_queue.insert (m_queue.end (), track_.cbegin (), track_.cend ());

    m_currentPos = m_queue.begin () + dist;

    queueUpdated (m_queue);
}

muuid::uuid const &queue_t::pop ()
{
    auto const &id = *m_currentPos;

    m_currentPos = m_queue.erase (m_currentPos);

    return id;
}

bool queue_t::empty () const
{
    return m_queue.empty ();
}

std::span<muuid::uuid const> queue_t::currentQueue () const
{
    return m_queue;
}

std::optional<muuid::uuid> queue_t::currentTrack () const
{
    if (m_currentPos != m_queue.end ())
        return *m_currentPos;

    return {};
}

size_t queue_t::pos () const
{
    return std::ranges::distance (m_queue.begin (), m_currentPos);
}

std::optional<muuid::uuid> queue_t::moveForward ()
{
    ++m_currentPos;

    if (m_currentPos == m_queue.end ())
        m_currentPos = m_queue.begin ();

    return *m_currentPos;
}

std::optional<muuid::uuid> queue_t::moveBack ()
{
    if (m_currentPos == m_queue.begin ())
        m_currentPos = m_queue.end ();

    --m_currentPos;

    return *m_currentPos;
}

std::optional<muuid::uuid> queue_t::lookAhead ()
{
    auto tmp = m_currentPos;
    ++tmp;

    if (tmp == m_queue.end ())
        tmp = m_queue.begin ();

    return *tmp;
}

std::optional<muuid::uuid> queue_t::lookBack ()
{
    if (m_currentPos == m_queue.begin ())
        return *(--m_queue.end ());

    auto tmp = m_currentPos;
    --tmp;

    return *tmp;
}

std::optional<muuid::uuid> queue_t::skipTo (size_t pos_)
{
    if (pos_ >= m_queue.size ())
        return {};

    m_currentPos = m_queue.begin () + pos_;

    return *m_currentPos;
}
