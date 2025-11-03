#pragma once

#include "clipod/database.h"
#include "clipod/playback.h"
#include "clipod/queue.h"
#include "clipod/settings.h"

namespace clipod
{

struct brain_t
{
    static std::shared_ptr<brain_t> bb ()
    {
        static std::shared_ptr<brain_t> bigBrain;

        if (bigBrain)
            return bigBrain;

        bigBrain = std::make_shared<brain_t> ();
        bigBrain->db = std::make_shared<database_t> ();
        bigBrain->qu = std::make_shared<queue_t> ();
        bigBrain->pb = std::make_shared<playback_t> ();
        bigBrain->settings = std::make_shared<settings_t> ();

        return bigBrain;
    }

    std::shared_ptr<database_t> db;
    std::shared_ptr<queue_t> qu;
    std::shared_ptr<playback_t> pb;
    std::shared_ptr<settings_t> settings;
};

inline std::shared_ptr<brain_t> BB ()
{
    return brain_t::bb ();
}

inline std::shared_ptr<database_t> DB ()
{
    return brain_t::bb ()->db;
}

inline std::shared_ptr<queue_t> QU ()
{
    return brain_t::bb ()->qu;
}

inline std::shared_ptr<playback_t> PB ()
{
    return brain_t::bb ()->pb;
}

inline std::shared_ptr<settings_t> SETTINGS ()
{
    return brain_t::bb ()->settings;
}

}; // namespace clipod
