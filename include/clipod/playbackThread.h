#pragma once

namespace clipod
{
class playbackThread_t
{
public:
    ~playbackThread_t ();
    playbackThread_t ();

    void work ();
};
} // namespace clipod
