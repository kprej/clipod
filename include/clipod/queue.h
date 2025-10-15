#pragma once

#include <sigslot/signal.hpp>

#include <modern-uuid/uuid.h>

#include <optional>
#include <vector>

namespace clipod
{
class queue_t
{
public:
    ~queue_t ();
    queue_t ();

    void enqueue (muuid::uuid const &track_);
    void enqueue (std::vector<muuid::uuid> const &tracks_);

    muuid::uuid const &pop ();

    bool empty () const;

    std::span<muuid::uuid const> currentQueue () const;
    std::optional<muuid::uuid> currentTrack () const;

    size_t pos () const;

    std::optional<muuid::uuid> moveForward ();
    std::optional<muuid::uuid> moveBack ();

    std::optional<muuid::uuid> lookAhead ();
    std::optional<muuid::uuid> lookBack ();

    std::optional<muuid::uuid> skipTo (size_t pos_);

    sigslot::signal<std::span<muuid::uuid>> queueUpdated;

private:
    std::vector<muuid::uuid> m_queue;
    std::vector<muuid::uuid>::iterator m_currentPos;
};
} // namespace clipod
